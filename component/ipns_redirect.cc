#include "ipns_redirect.h"

#include "inter_request_state.h"

#include "net/base/net_errors.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/mojom/network_context.mojom.h"
#include "services/network/public/mojom/url_loader.mojom.h"
#include "services/network/public/mojom/url_response_head.mojom.h"

namespace moj = network::mojom;

ipfs::IpnsRedirect::IpnsRedirect(InterRequestState& state,
                                 std::string host,
                                 moj::NetworkContext* network_context)
    : state_{state}, host_(host) {
  DCHECK(network_context);
  auto prev = state_.NameResolvedTo(host);
  if (prev.empty()) {
    auto params = moj::ResolveHostParameters::New();
    params->dns_query_type = net::DnsQueryType::TXT;
    params->initial_priority = net::RequestPriority::HIGHEST;
    params->source = net::HostResolverSource::ANY;
    params->cache_usage = moj::ResolveHostParameters_CacheUsage::STALE_ALLOWED;
    params->secure_dns_policy = moj::SecureDnsPolicy::ALLOW;
    params->purpose = moj::ResolveHostParameters::Purpose::kUnspecified;

    auto hrh = moj::HostResolverHost::NewHostPortPair({"_dnslink." + host, 0});
    auto nak = net::NetworkAnonymizationKey::CreateTransient();
    network_context->ResolveHost(std::move(hrh), nak, std::move(params),
                                 recv_.BindNewPipeAndPassRemote());
  }
}
ipfs::IpnsRedirect::~IpnsRedirect() noexcept {}

void ipfs::IpnsRedirect::DoRedirect(
    std::shared_ptr<IpnsRedirect> me,
    network::ResourceRequest const& resource_request,
    mojo::PendingReceiver<network::mojom::URLLoader>,
    mojo::PendingRemote<network::mojom::URLLoaderClient> client) {
  me->lifetime_extension_ = me;
  me->requested_uri_ = resource_request.url;
  me->client_remote_ = std::move(client);
  //  me->loader_receiver_ = std::move(receiver);
  auto repl = me->state_.NameResolvedTo(me->host_);
  if (repl.size()) {
    me->CreateRedirect();
  }
}

void ipfs::IpnsRedirect::OnTextResults(
    std::vector<std::string> const& text_results) {
  constexpr std::string_view prefix{"dnslink=/"};
  for (auto& text_result : text_results) {
    if (text_result.compare(0, prefix.size(), prefix)) {
      LOG(INFO) << "Irrelevant text result: " << text_result;
    } else {
      auto replacement = std::string_view{text_result}.substr(prefix.size());
      state_.AssignName(host_, std::string{replacement});
      if (requested_uri_) {
        CreateRedirect();
      }
      return;
    }
  }
  // TODO create some sort of host-not-found-like error
}
void ipfs::IpnsRedirect::OnComplete(
    int32_t result,
    ::net::ResolveErrorInfo const&,
    absl::optional<::net::AddressList> const&,
    absl::optional<std::vector<::net::HostResolverEndpointResult>> const&) {
  if (result != net::Error::OK) {
    LOG(ERROR) << "Error resolving _dnslink." << host_ << " : " << result;
    // TODO - create http error response
  }
  lifetime_extension_.reset();
}
void ipfs::IpnsRedirect::CreateRedirect() {
  mojo::Remote<network::mojom::URLLoaderClient> client;
  client.Bind(std::move(client_remote_));
  auto from_url = requested_uri_.value();
  std::string to{state_.NameResolvedTo(host_)};
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
  LOG(WARNING) << "Redirecting " << from_url << " to " << to_url;

  auto redirect = net::RedirectInfo::ComputeRedirectInfo(
      net::HttpRequestHeaders::kGetMethod, requested_uri_.value(),
      net::SiteForCookies::FromUrl(requested_uri_.value()),
      net::RedirectInfo::FirstPartyURLPolicy::UPDATE_URL_ON_REDIRECT,
      net::ReferrerPolicy::NO_REFERRER, requested_uri_.value().spec(), 302,
      to_url, absl::nullopt, false, false, false);
  client->OnReceiveRedirect(redirect, network::mojom::URLResponseHead::New());
  client->OnComplete(network::URLLoaderCompletionStatus{308});
}