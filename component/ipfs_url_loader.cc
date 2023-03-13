#include "ipfs_url_loader.h"

#include "inter_request_state.h"

#include "ipfs_client/gateways.h"
#include "ipfs_client/unixfs_path_resolver.h"

#include "base/notreached.h"
#include "base/strings/escape.h"
#include "base/threading/platform_thread.h"
#include "net/base/mime_sniffer.h"
#include "net/base/mime_util.h"
#include "services/network/public/cpp/parsed_headers.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "services/network/url_loader_factory.h"

#include <libp2p/multi/content_identifier_codec.hpp>

#include <fstream>

ipfs::IpfsUrlLoader::IpfsUrlLoader(
    network::mojom::URLLoaderFactory& handles_http,
    InterRequestState& state)
    : state_{state},
      lower_loader_factory_{handles_http},
      sched_(state_.scheduler()) {}

ipfs::IpfsUrlLoader::~IpfsUrlLoader() noexcept {
  LOG(INFO) << "loader go bye-bye";
}

void ipfs::IpfsUrlLoader::FollowRedirect(
    std::vector<std::string> const&  // removed_headers
    ,
    net::HttpRequestHeaders const&  // modified_headers
    ,
    net::HttpRequestHeaders const&  // modified_cors_exempt_headers
    ,
    absl::optional<::GURL> const&  // new_url
) {
  NOTIMPLEMENTED();
}

void ipfs::IpfsUrlLoader::SetPriority(net::RequestPriority priority,
                                      int32_t intra_priority_value) {
  LOG(INFO) << "TODO SetPriority(" << priority << ',' << intra_priority_value
            << ')';
}

void ipfs::IpfsUrlLoader::PauseReadingBodyFromNet() {
  NOTIMPLEMENTED();
}

void ipfs::IpfsUrlLoader::ResumeReadingBodyFromNet() {
  NOTIMPLEMENTED();
}

void ipfs::IpfsUrlLoader::StartRequest(
    std::shared_ptr<IpfsUrlLoader> me,
    network::ResourceRequest const& resource_request,
    mojo::PendingReceiver<network::mojom::URLLoader> receiver,
    mojo::PendingRemote<network::mojom::URLLoaderClient> client) {
  DCHECK(!me->receiver_.is_bound());
  DCHECK(!me->client_.is_bound());
  me->receiver_.Bind(std::move(receiver));
  me->client_.Bind(std::move(client));
  if (me->original_url_.empty()) {
    me->original_url_ = resource_request.url.spec();
  }
  if (resource_request.url.SchemeIs("ipfs")) {
    auto ref = resource_request.url.spec();
    DCHECK_EQ(ref.substr(0, 7), "ipfs://");
    // TODO these kinds of shenanigans should have their own special utils file
    ref.erase(4, 2);
    me->StartUnixFsProc(me, ref);
  } else {
    LOG(ERROR) << "Wrong scheme: " << resource_request.url.scheme();
  }
}

void ipfs::IpfsUrlLoader::StartUnixFsProc(ptr me, std::string_view ipfs_ref) {
  LOG(INFO) << "Requesting " << ipfs_ref << " by blocks.";
  DCHECK_EQ(ipfs_ref.substr(0, 5), "ipfs/");
  auto second_slash = ipfs_ref.find_first_of("/?", 5);
  auto cid = ipfs_ref.substr(5, second_slash - 5);
  second_slash = ipfs_ref.find('/', 5);
  auto qmark = ipfs_ref.find('?');
  std::string remainder{"/"};
  if (second_slash < ipfs_ref.size()) {
    remainder.assign(ipfs_ref.substr(second_slash + 1));
  } else if (qmark && qmark < ipfs_ref.size()) {
    remainder.append(ipfs_ref.substr(qmark));
  }
  LOG(INFO) << "cid=" << cid << " remainder=" << remainder;
  me->resolver_ = std::make_shared<UnixFsPathResolver>(
      me->state_.storage(), std::string{cid}, remainder);
  me->resolver_->Step(me);
}

void ipfs::IpfsUrlLoader::OverrideUrl(GURL u) {
  original_url_ = u.spec();
}

constexpr net::NetworkTrafficAnnotationTag kTrafficAnnotation =
    net::DefineNetworkTrafficAnnotation("ipfs_gateway_request", R"(
      semantics {
        sender: "IPFS component"
        description:
          "Sends a request to an IPFS gateway for a block."
        trigger:
          "Processing of an ipfs:// URL (possibly from an ipns:// URL)."
        data: "None"
        destination: WEBSITE
      }
      policy {
        cookies_allowed: NO
        setting: "Currently, this feature cannot be disabled by settings. TODO"
      }
    )");

void ipfs::IpfsUrlLoader::RequestByCid(std::string cid,
                                       Scheduler::Priority prio) {
  LOG(INFO) << __PRETTY_FUNCTION__ << " (" << cid << ',' << prio << ").";
  if (complete_) {
    LOG(INFO) << "Not creating block request because we're completed.";
    return;
  }
  sched_->Enqueue(shared_from_this(), "ipfs/" + cid + "?format=raw", prio);
}

void ipfs::IpfsUrlLoader::InitiateGatewayRequest(BusyGateway assigned) {
  if (complete_) {
    return;
  }
  auto url_suffix = assigned->current_task();
  auto req = std::make_unique<network::ResourceRequest>();
  auto url = assigned->url();
  LOG(INFO) << "Starting a gateway request: " << url;
  req->url = GURL{url};
  auto idx = gateway_requests_.size();
  gateway_requests_.emplace_back(
      std::move(assigned), network::SimpleURLLoader::Create(
                               std::move(req), kTrafficAnnotation, FROM_HERE));
  auto cb = base::BindOnce(&ipfs::IpfsUrlLoader::OnGatewayResponse,
                           base::Unretained(this), shared_from_this(), idx);
  // TODO - proper requesting with full features (pause, explict cancel, etc.).
  // May require not using SimpleURLLoader
  gateway_requests_.back()
      .second->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
          &lower_loader_factory_, std::move(cb));
}

void ipfs::IpfsUrlLoader::OnGatewayResponse(std::shared_ptr<ipfs::FrameworkApi>,
                                            std::size_t req_idx,
                                            std::unique_ptr<std::string> body) {
  auto it = std::next(gateway_requests_.begin(), req_idx);
  auto& gw = it->first;
  LOG(INFO) << "OnGatewayResponse(" << req_idx << ")=" << gw->url();
  auto& http_loader = it->second;
  CHECK(gw);
  CHECK(http_loader);
  // Doing a deep copy to avoid reference invalidation concerns below
  auto prefix = gw->url_prefix();
  auto url = gw->url();
  auto task = gw->current_task();
  if (ProcessBlockResponse(*gw, http_loader.get(), body.get())) {
    LOG(INFO) << "Promoting " << prefix << " due to success of " << url;
    gw.Success(state_.gateways());
    LOG(INFO) << "Cancelling all other requests for " << task;
    for (auto& gr : gateway_requests_) {
      if (!gr.first) {
        gr.second.reset();
      } else if (!gr.second) {
        gr.first.reset();
      } else if (url == gr.first->url()) {
        LOG(INFO) << "Skip self";
      } else if (task == gr.first->current_task()) {
        gr.first.reset();
        gr.second.reset();
      }
    }
    if (!complete_) {
      sched_->IssueRequests(shared_from_this());
    }
  } else {
    LOG(INFO) << "Demoting " << prefix;
    gw.Failure(state_.gateways());
    auto failure = sched_->DetectCompleteFailure();
    if (complete_) {
      LOG(ERROR) << "Already complete! Stop it!";
    } else if (failure.empty()) {
      LOG(INFO) << "Trying other gateways.";
      sched_->IssueRequests(shared_from_this());
    } else {
      LOG(ERROR) << "Run out of gateways to try for " << failure;
      // Creative reinterpretation of 'gateway' in 'bad gateway'
      client_->OnComplete(network::URLLoaderCompletionStatus{502});
      complete_ = true;
    }
  }
}

bool ipfs::IpfsUrlLoader::ProcessBlockResponse(Gateway& gw,
                                               network::SimpleURLLoader* gw_req,
                                               std::string* body) {
  LOG(INFO) << "ProcessBlockResponse(" << gw.url() << ')';
  if (!body) {
    LOG(INFO) << "ProcessBlockResponse(" << gw.url()
              << ") Null body - presumably http error.\n";
    return false;
  }
  network::mojom::URLResponseHead const* head = gw_req->ResponseInfo();
  if (!head) {
    LOG(INFO) << "ProcessBlockResponse(" << gw.url() << ") Null head.\n";
    return false;
  }
  std::string reported_path;
  head->headers->EnumerateHeader(nullptr, "X-Ipfs-Path", &reported_path);
  if (reported_path.empty()) {
    LOG(INFO) << "ProcessBlockResponse(" << gw.url()
              << ") gave us a non-ipfs response.\n";
    return false;
  }
  if (gw.url().find("?format=raw") != std::string::npos) {
    return HandleBlockResponse(gw, *body, *head);
  }
  ABSL_UNREACHABLE();  // TODO - if this remains true, remove dead code.
  LOG(INFO) << "Request for full file ProcessBlockResponse(" << gw.url()
            << ", body has " << body->size() << " bytes.)";
  if (complete_) {
    LOG(ERROR) << "Already complete! Stop it!";
    return true;
  }
  auto result = mojo::CreateDataPipe(body->size(), pipe_prod_, pipe_cons_);
  if (result) {
    LOG(ERROR) << " ERROR: TaskFailed to create data pipe: " << result;
    return false;
  }
  std::uint32_t write_size = body->size();
  pipe_prod_->WriteData(body->c_str(), &write_size,
                        MOJO_BEGIN_WRITE_DATA_FLAG_ALL_OR_NONE);

  auto head_out = head->Clone();

  head_out->mime_type = head->mime_type;
  head_out->content_length = write_size;
  head_out->headers->SetHeader("access-control-allow-origin", "*");
  head_out->headers->RemoveHeader("content-security-policy");
  head_out->headers->RemoveHeader("referrer-policy");
  head_out->headers->RemoveHeader("link");
  if (original_url_.size() > 2 &&
      original_url_.at(original_url_.size() - 2UL) == '2' &&
      original_url_.back() == 'F') {
    original_url_.append("index.html");  // TODO This isn't quite right.
  }
  head_out->parsed_headers =
      network::PopulateParsedHeaders(head->headers.get(), GURL{original_url_});
  head_out->was_fetched_via_spdy = false;
  client_->OnReceiveResponse(std::move(head_out), std::move(pipe_cons_),
                             absl::nullopt);
  client_->OnComplete(network::URLLoaderCompletionStatus{});
  complete_ = true;
  return true;
}

bool ipfs::IpfsUrlLoader::HandleBlockResponse(
    Gateway& gw,
    std::string const& body,
    network::mojom::URLResponseHead const& head) {
  LOG(INFO) << "Got a response to a block request from " << gw.url();
  std::string reported_content_type;
  head.headers->EnumerateHeader(nullptr, "Content-Type",
                                &reported_content_type);
  if (reported_content_type != "application/vnd.ipld.raw") {
    LOG(ERROR) << '\n'
               << gw.url() << " reported a content type of "
               << reported_content_type
               << " strongly implying that it's a full request, not a single "
                  "block. TODO: Remove "
               << gw.url_prefix() << " from list of gateways?\n";
    state_.gateways().demote(gw.url_prefix());
    return false;
  }
  auto cid_str = gw.current_task();
  cid_str.erase(0, 5);  // ipfs/
  cid_str.erase(cid_str.find('?'));
  auto cid = libp2p::multi::ContentIdentifierCodec::fromString(cid_str);
  if (!cid.has_value()) {
    LOG(ERROR) << "Invalid CID '" << cid_str << "'.";
    return false;
  }
  Block block{cid.value(), body};
  if (block.cid_matches_data()) {
    LOG(INFO) << "Storing CID=" << cid_str;
    state_.storage().Store(shared_from_this(), cid_str, std::move(block));
    resolver_->Step(shared_from_this());
    sched_->IssueRequests(shared_from_this());
    return true;
  } else {
    LOG(ERROR) << "You tried to store some bytes as a block for a CID ("
               << cid_str << ") that does not correspond to those bytes!";
    // TODO ban the gateway outright
    return false;
  }
}

void ipfs::IpfsUrlLoader::BlocksComplete(std::string mime_type) {
  LOG(INFO) << "Resolved from unix-fs dag a file of type: " << mime_type
            << " will report it as " << original_url_;
  if (complete_) {
    return;
  }
  auto result =
      mojo::CreateDataPipe(partial_block_.size(), pipe_prod_, pipe_cons_);
  if (result) {
    LOG(ERROR) << " ERROR: TaskFailed to create data pipe: " << result;
    return;
  }
  auto head = network::mojom::URLResponseHead::New();
  head->mime_type = mime_type;
  std::uint32_t byte_count = partial_block_.size();
  pipe_prod_->WriteData(partial_block_.data(), &byte_count,
                        MOJO_BEGIN_WRITE_DATA_FLAG_ALL_OR_NONE);
  head->content_length = byte_count;
  head->headers =
      net::HttpResponseHeaders::TryToCreate("access-control-allow-origin: *");
  if (!head->headers) {
    LOG(ERROR) << "\n\tFailed to create headers!\n";
  }
  head->parsed_headers =
      network::PopulateParsedHeaders(head->headers.get(), GURL{original_url_});
  head->was_fetched_via_spdy = false;
  client_->OnReceiveResponse(std::move(head), std::move(pipe_cons_),
                             absl::nullopt);
  client_->OnComplete(network::URLLoaderCompletionStatus{});
  complete_ = true;
}

void ipfs::IpfsUrlLoader::FourOhFour(std::string_view cid,
                                     std::string_view path) {
  LOG(ERROR) << "Immutable data 404 for " << cid << '/' << path;
  complete_ = true;
  client_->OnComplete(network::URLLoaderCompletionStatus{404});
  gateway_requests_.clear();
}

std::string ipfs::IpfsUrlLoader::MimeType(std::string extension,
                                          std::string_view content,
                                          std::string const& url) const {
  std::string result;
  if (extension.size() &&
      net::GetWellKnownMimeTypeFromExtension(extension, &result)) {
    LOG(INFO) << "Got " << result << " from extension " << extension << " for "
              << url;
  } else {
    result.clear();
  }
  if (net::SniffMimeType({content.data(), content.size()}, GURL{url}, result,
                         net::ForceSniffFileUrlsForHtml::kDisabled, &result)) {
    LOG(INFO) << "Got " << result << " from content of " << url;
  }
  return result;
}

void ipfs::IpfsUrlLoader::ReceiveBlockBytes(std::string_view content) {
  // TODO - cid? We may have more than one block in flight at the same time
  partial_block_.append(content);
}

std::string ipfs::IpfsUrlLoader::UnescapeUrlComponent(
    std::string_view comp) const {
  using Rule = base::UnescapeRule;
  auto rules = Rule::PATH_SEPARATORS |
               Rule::URL_SPECIAL_CHARS_EXCEPT_PATH_SEPARATORS | Rule::SPACES;
  auto result = base::UnescapeURLComponent({comp.data(), comp.size()}, rules);
  LOG(INFO) << "UnescapeUrlComponent(" << comp << ")->'" << result << "'";
  return result;
}

std::string ipfs::IpfsUrlLoader::GetIpfsRefFromIpnsUri(
    std::string_view replacement) const {
  auto spec = original_url_;
  // https://docs.ipfs.tech/concepts/dnslink/
  // The rest can be an /ipfs/ link (as in the example below), or /ipns/, or
  // even a link to another DNSLink.
  DCHECK_EQ(spec.substr(0, 7), "ipns://");
  auto end_of_name = spec.find('/', 8);
  LOG(INFO) << "IPNS name part of URL extends through " << end_of_name;
  if (end_of_name < spec.size()) {
    spec.replace(0, end_of_name, replacement);

    return spec;
  } else {
    return std::string{replacement};
  }
}
