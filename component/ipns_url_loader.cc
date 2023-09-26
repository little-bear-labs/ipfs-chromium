#include "ipns_url_loader.h"

#include "gateway_requests.h"
#include "inter_request_state.h"

#include "net/base/net_errors.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/mojom/network_context.mojom.h"
#include "services/network/public/mojom/url_loader.mojom.h"
#include "services/network/public/mojom/url_response_head.mojom.h"

#include <ipfs_client/context_api.h>

#include <libp2p/multi/content_identifier_codec.hpp>
#include <libp2p/peer/peer_id.hpp>

namespace moj = network::mojom;

ipfs::IpnsUrlLoader::IpnsUrlLoader(
    InterRequestState& state,
    std::string host,
    moj::NetworkContext* network_context,
    network::mojom::URLLoaderFactory& handles_http)
    : state_{state},
      host_(host),
      ipfs_loader_(std::make_shared<ipfs::IpfsUrlLoader>(handles_http, state)),
      network_context_{network_context},
      http_loader_{handles_http} {
  DCHECK(network_context);
}
ipfs::IpnsUrlLoader::~IpnsUrlLoader() noexcept {}

void ipfs::IpnsUrlLoader::StartHandling(
    std::shared_ptr<IpnsUrlLoader> me,
    network::ResourceRequest const& resource_request,
    mojo::PendingReceiver<network::mojom::URLLoader> receiver,
    mojo::PendingRemote<network::mojom::URLLoaderClient> client) {
  me->ipfs_loader_->extra(me);
  me->request_ = resource_request;
  me->client_remote_ = std::move(client);
  me->loader_receiver_ = std::move(receiver);
  me->Next();
}

void ipfs::IpnsUrlLoader::OnTextResults(
    std::vector<std::string> const& text_results) {
  LOG(INFO) << "There are " << text_results.size() << " text records.";
  constexpr std::string_view prefix{"dnslink=/"};
  // https://dnslink.io/#multiple-records
  std::string result;
  for (auto& text_result : text_results) {
    if (text_result.compare(0, prefix.size(), prefix)) {
      LOG(INFO) << "Irrelevant text result: " << text_result;
    } else {
      auto replacement = std::string_view{text_result}.substr(prefix.size());
      LOG(INFO) << "Text result '" << text_result << "' -> replacment string '"
                << replacement << "'.";
      if (replacement.substr(0, 2) != "ip") {
        LOG(ERROR) << "Unsupported DNSLink redirect '" << replacement << "'.";
      } else if (replacement.substr(3, 2) != "s/") {
      } else if (result.empty() || replacement < result) {
        // Note that "ipfs/..." < "ipns/..."
        result.assign(replacement);
      }
    }
  }
  if (result.empty()) {
    state_->names().NoSuchName(host_);
    LOG(ERROR)
        << "_dnslink. domain exists, but contains no /ipfs or /ipns entry";
  } else {
    state_->names().AssignDnsLink(host_, result);
  }
}
void ipfs::IpnsUrlLoader::OnComplete(
    int32_t result,
    ::net::ResolveErrorInfo const&,
    absl::optional<::net::AddressList> const&,
    absl::optional<std::vector<::net::HostResolverEndpointResult>> const&) {
  LOG(INFO) << "Done with a DNS request.";
  auto _ = recv_.Unbind();
  if (result == net::Error::OK) {
    Next();
  } else {
    LOG(ERROR) << "Error resolving _dnslink." << host_ << " : " << result;
    state_->names().NoSuchName(host_);
    FailNameResolution();
  }
}
void ipfs::IpnsUrlLoader::Next() {
  auto resolved = state_->names().NameResolvedTo(host_);
  if (resolved.empty()) {
    if (!RequestIpnsRecord()) {
      LOG(INFO) << "Treating '" << host_ << "' as a DNSLink host.";
      QueryDns(host_);
    }
  } else if (resolved == IpnsNames::kNoSuchName) {
    LOG(WARNING) << "We have given up on resolving DNSLink " << host_;
    FailNameResolution();
  } else if (request_ && resolved.substr(0, 5) == "ipfs/") {
    DoIpfs();
  } else if (resolved.substr(0, 5) == "ipns/") {
    LOG(INFO) << "Moving an indirection down from " << host_ << " to "
              << resolved;
    host_.assign(resolved, 5);
    Next();
  }
}
void ipfs::IpnsUrlLoader::DoIpfs() {
  auto resolved = state_->names().NameResolvedTo(host_);
  auto from_url = request_.value().url;
  std::string to{resolved};
  DCHECK_GT(to.size(), 5U);
  DCHECK_EQ(to[0], 'i');
  DCHECK_EQ(to[1], 'p');
  DCHECK(to[2] == 'f' || to[2] == 'n');
  DCHECK_EQ(to[3], 's');
  DCHECK_EQ(to[4], '/');
  to.insert(4, ":/");
  if (from_url.has_path()) {
    to.append(from_url.path());
  }
  if (from_url.has_query()) {
    // Stuff following '?' up to the ref. The getters will not include the '?'.
    to.push_back('?');
    to.append(from_url.query());
  }
  if (from_url.has_ref()) {
    // Stuff following '#' to the end of the string. This will be %-escaped
    // UTF-8. The getters will not include the '#'.
    to.push_back('#');
    to.append(from_url.ref());
  }
  GURL to_url{to};
  VLOG(1) << "Treating " << from_url << " as " << to_url;
  ipfs_loader_->OverrideUrl(from_url);
  auto* entry = state_->names().Entry(host_);
  if (entry) {
    auto duration = entry->resolution_ms;
    ipfs_loader_->AddHeader(
        "Server-Timing",
        "ipns;desc=\"IPNS record request\";dur=" + std::to_string(duration));
    ipfs_loader_->AddHeader(
        "IPNS-Name-Source",
        entry->gateway_source + " @" + std::to_string(entry->fetch_time));
  }
  request_.value().url = to_url;
  ipfs::IpfsUrlLoader::StartRequest(ipfs_loader_, *request_,
                                    std::move(loader_receiver_),
                                    std::move(client_remote_));
  ipfs_loader_->extra({});
}

network::mojom::URLLoader& ipfs::IpnsUrlLoader::under() {
  return *ipfs_loader_;
}

void ipfs::IpnsUrlLoader::FollowRedirect(
    std::vector<std::string> const& removed_headers,
    ::net::HttpRequestHeaders const& modified_headers,
    ::net::HttpRequestHeaders const& modified_cors_exempt_headers,
    absl::optional<::GURL> const& new_url) {
  under().FollowRedirect(removed_headers, modified_headers,
                         modified_cors_exempt_headers, new_url);
}
void ipfs::IpnsUrlLoader::SetPriority(::net::RequestPriority priority,
                                      int32_t intra_priority_value) {
  under().SetPriority(priority, intra_priority_value);
}
void ipfs::IpnsUrlLoader::PauseReadingBodyFromNet() {
  under().PauseReadingBodyFromNet();
}
void ipfs::IpnsUrlLoader::ResumeReadingBodyFromNet() {
  under().ResumeReadingBodyFromNet();
}
void ipfs::IpnsUrlLoader::QueryDns(std::string_view host) {
  if (recv_.is_bound()) {
    LOG(INFO) << "Awaiting DNS response.";
    return;
  }
  auto params = moj::ResolveHostParameters::New();
  params->dns_query_type = net::DnsQueryType::TXT;
  params->initial_priority = net::RequestPriority::HIGHEST;
  params->source = net::HostResolverSource::ANY;
  params->cache_usage = moj::ResolveHostParameters_CacheUsage::STALE_ALLOWED;
  params->secure_dns_policy = moj::SecureDnsPolicy::ALLOW;
  params->purpose = moj::ResolveHostParameters::Purpose::kUnspecified;
  std::string dnslink_host{"_dnslink."};
  dnslink_host.append(host);
  LOG(INFO) << "Querying DNS for TXT records on '" << dnslink_host
            << "' in service of resolving "
            << (request_ ? request_->url.spec() : host_);
  auto hrh = moj::HostResolverHost::NewHostPortPair({dnslink_host, 0});
  auto nak = net::NetworkAnonymizationKey::CreateTransient();
  network_context_->ResolveHost(std::move(hrh), nak, std::move(params),
                                recv_.BindNewPipeAndPassRemote());
}
void ipfs::IpnsUrlLoader::FailNameResolution() {
  if (client_remote_.is_valid()) {
    mojo::Remote<network::mojom::URLLoaderClient> client;
    client.Bind(std::move(client_remote_));
    client->OnComplete(
        network::URLLoaderCompletionStatus(net::ERR_NAME_NOT_RESOLVED));
  }
}
bool ipfs::IpnsUrlLoader::RequestIpnsRecord() {
  auto cid = libp2p::multi::ContentIdentifierCodec::fromString(host_);
  if (!cid.has_value()) {
    return false;
  }
  if (cid.value().content_type !=
      libp2p::multi::MulticodecType::Code::LIBP2P_KEY) {
    return false;
  }
  if (api_) {
    // true because this is true IPNS
    //  ... but return early because we have already requested it
    state_->scheduler().IssueRequests(api_);
    return true;
  }
  auto key = "ipns/" + host_;
  auto caches = state_->serialized_caches();
  auto check =
      [this, key](
          std::function<void()> fail,
          std::shared_ptr<CacheRequestor> cache) -> std::function<void()> {
    return [this, key, fail, cache]() {
      LOG(INFO) << "Attempting to fetch IPNS record from " << cache->name();
      auto hit = [this, cache](auto b, auto) { CacheHit(cache, b); };
      cache->FetchEntry(key, net::HIGHEST, hit, fail);
    };
  };
  std::function<void()> last_resort = [this]() {
    LOG(INFO) << "All caches missed for " << this->host_;
    this->RequestFromGateway();
  };
  auto chain =
      std::accumulate(caches.rbegin(), caches.rend(), last_resort, check);
  chain();
  return true;
}
void ipfs::IpnsUrlLoader::CacheHit(std::shared_ptr<CacheRequestor> cache,
                                   std::string_view body) {
  LOG(INFO) << "IPNS cache hit! " << host_ << "=" << body;
  auto result = ValidatedIpns::Deserialize(std::string{body});
  if (result.use_until < std::time(nullptr)) {
    LOG(WARNING) << "Unfortunately, the cache entry for " << host_
                 << " is too old - it expired at " << result.use_until;
    cache->Expire("ipns/" + host_);
    RequestFromGateway();
  } else {
    state_->names().AssignName(host_, result);
    Complete();
  }
}
void ipfs::IpnsUrlLoader::RequestFromGateway() {
  api_ = state_->api();
  api_->SetLoaderFactory(*http_loader_);
  state_->scheduler().Enqueue(api_, {}, shared_from_this(),
                              "ipns/" + host_ + "?format=ipns-record", 3);
  state_->scheduler().IssueRequests(api_);
}
void ipfs::IpnsUrlLoader::Complete() {
  LOG(INFO) << "NameListener's Complete called for an IpnsLoader.";
  auto* entry = state_->names().Entry(host_);
  if (entry) {
    auto caches = state_->serialized_caches();
    LOG(INFO) << "Storing the resolution of ipns://" << host_ << " in "
              << caches.size() << " cache backends.";
    for (auto cache : caches) {
      cache->Store("ipns/" + host_, "IPNS", entry->Serialize());
    }
    Next();
  } else {
    LOG(ERROR) << "Failed to resolve IPNS " << host_;
    FailNameResolution();
  }
}
