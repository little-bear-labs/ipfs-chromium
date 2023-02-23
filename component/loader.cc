#include "loader.h"

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

#include <fstream>

ipfs::Loader::Loader(network::mojom::URLLoaderFactory* handles_http,
                     InterRequestState& state)
    : state_{state},
      lower_loader_factory_{handles_http},
      sched_(state_.gateways().GenerateList()) {}
ipfs::Loader::~Loader() noexcept {}

void ipfs::Loader::FollowRedirect(
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
void ipfs::Loader::SetPriority(net::RequestPriority priority,
                               int32_t intra_priority_value) {
  LOG(INFO) << "TODO SetPriority(" << priority << ',' << intra_priority_value
            << ')';
}
void ipfs::Loader::PauseReadingBodyFromNet() {
  NOTIMPLEMENTED();
}
void ipfs::Loader::ResumeReadingBodyFromNet() {
  NOTIMPLEMENTED();
}

void ipfs::Loader::StartRequest(
    std::shared_ptr<Loader> me,
    network::ResourceRequest const& resource_request,
    mojo::PendingReceiver<network::mojom::URLLoader> receiver,
    mojo::PendingRemote<network::mojom::URLLoaderClient> client) {
  DCHECK(!me->receiver_.is_bound());
  DCHECK(!me->client_.is_bound());
  me->receiver_.Bind(std::move(receiver));
  me->client_.Bind(std::move(client));
  me->original_url_ = resource_request.url.spec();
  std::string path;
  if (resource_request.url.SchemeIs("ipfs") ||
      resource_request.url.SchemeIs("ipns")) {
    path = resource_request.url.spec();
    path.erase(4, 2);
  } else {
    LOG(FATAL) << "Removed support for ipfs-over-http?";
  }
  if (resource_request.url.SchemeIs("ipfs")) {
    LOG(INFO) << "Requesting " << resource_request.url.spec() << " by blocks!";
    auto second_slash = path.find_first_of("/?", 5);
    auto cid = path.substr(5, second_slash - 5);
    second_slash = path.find('/', 5);
    auto qmark = path.find('?');
    std::string remainder{"/"};
    if (second_slash < path.size()) {
      remainder.assign(path.substr(second_slash + 1));
    } else if (qmark && qmark < path.size()) {
      remainder.append(path.substr(qmark));
    }
    LOG(INFO) << "cid=" << cid << " remainder=" << remainder;
    me->resolver_ = std::make_shared<UnixFsPathResolver>(me->state_.storage(),
                                                         cid, remainder);
    me->resolver_->Step(me);
  } else {
    LOG(ERROR) << "Requesting " << resource_request.url.spec()
               << " as a whole file.";
    me->sched_.Enqueue(me, path, Scheduler::Priority::Required);
  }
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
void ipfs::Loader::RequestByCid(std::string cid, Scheduler::Priority prio) {
  LOG(INFO) << __PRETTY_FUNCTION__ << " (" << cid << ',' << prio << ").";
  if (complete_) {
    LOG(INFO) << "Not creating block request because we're completed.";
    return;
  }
  if (prio == Scheduler::Priority::Optional) {
    LOG(INFO) << "prefetch " << cid;
    return;
  }
  sched_.Enqueue(shared_from_this(), "ipfs/" + cid + "?format=raw",
                 Scheduler::Priority::Required);
}
void ipfs::Loader::InitiateGatewayRequest(BusyGateway assigned) {
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
  auto cb = base::BindOnce(&ipfs::Loader::OnGatewayResponse,
                           base::Unretained(this), shared_from_this(), idx);
  // TODO - proper requesting with full features (pause, explict cancel, etc.).
  // May require not using SimpleURLLoader
  gateway_requests_.back()
      .second->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
          lower_loader_factory_, std::move(cb));
}
void ipfs::Loader::OnGatewayResponse(std::shared_ptr<ipfs::FrameworkApi>,
                                     std::size_t req_idx,
                                     std::unique_ptr<std::string> body) {
  auto it = std::next(gateway_requests_.begin(), req_idx);
  auto& gw = it->first;
  LOG(INFO) << "OnGatewayResponse(" << req_idx << ")=" << gw->url();
  auto& http_loader = it->second;
  if (!gw) {
    LOG(ERROR) << "Gateway null in response handling!";
    sched_.IssueRequests(shared_from_this());
    return;
  }
  if (!http_loader) {
    LOG(ERROR) << "http_loader null in response handling!";
    sched_.IssueRequests(shared_from_this());
    return;
  }
  // Doing a deep copy to avoid reference invalidation concerns below, for
  // simplicity.
  auto prefix = gw->url_prefix();
  auto url = gw->url();
  auto task = gw->current_task();
  if (handle_response(*gw, http_loader.get(), body.get())) {
    LOG(INFO) << "Promoting " << prefix << " due to success of " << url;
    gw.Success(state_.gateways());
    LOG(INFO) << "Cancelling all other requests for " << task;
    for (auto& gr : gateway_requests_) {
      if (!gr.first) {
        gr.second.reset();
      } else if (!gr.second) {
        gr.first.reset();
      } else if (task == gr.first->current_task()) {
        gr.first.reset();
        gr.second.reset();
      }
    }
    if (!complete_) {
      sched_.IssueRequests(shared_from_this());
    }
  } else {
    LOG(INFO) << "Demoting " << prefix;
    gw.Failure(state_.gateways());
    auto failure = sched_.DetectCompleteFailure();
    if (complete_) {
      LOG(ERROR) << "Already complete! Stop it!";
    } else if (failure.empty()) {
      LOG(INFO) << "Trying other gateways.";
      sched_.IssueRequests(shared_from_this());
    } else {
      LOG(ERROR) << "Run out of gateways to try for " << failure;
      // Creative reinterpretation of 'gateway' in 'bad gateway'
      client_->OnComplete(network::URLLoaderCompletionStatus{502});
      complete_ = true;
    }
  }
}
bool ipfs::Loader::handle_response(Gateway& gw,
                                   network::SimpleURLLoader* gw_req,
                                   std::string* body) {
  LOG(INFO) << "handle_response(" << gw.url() << ')';
  if (!body) {
    LOG(INFO) << "handle_response(" << gw.url()
              << ") Null body - presumably http error.\n";
    return false;
  }
  network::mojom::URLResponseHead const* head = gw_req->ResponseInfo();
  if (!head) {
    LOG(INFO) << "handle_response(" << gw.url() << ") Null head.\n";
    return false;
  }
  std::string reported_path;
  head->headers->EnumerateHeader(nullptr, "X-Ipfs-Path", &reported_path);
  if (reported_path.empty()) {
    LOG(INFO) << "handle_response(" << gw.url()
              << ") gave us a non-ipfs response.\n";
    return false;
  }
  if (gw.url().find("?format=raw") != std::string::npos) {
    return HandleBlockResponse(gw, *body, *head);
  }
  LOG(INFO) << "Request for full file handle_response(" << gw.url()
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
bool ipfs::Loader::HandleBlockResponse(
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
  auto cid = gw.current_task();
  cid.erase(0, 5);  // ipfs/
  cid.erase(cid.find('?'));
  LOG(INFO) << "Storing CID=" << cid;
  state_.storage().Store(shared_from_this(), cid, Block{body});
  resolver_->Step(shared_from_this());
  sched_.IssueRequests(shared_from_this());
  return true;
}

void ipfs::Loader::BlocksComplete(std::string mime_type) {
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
std::string ipfs::Loader::MimeType(std::string extension,
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
void ipfs::Loader::ReceiveBlockBytes(std::string_view content) {
  partial_block_.append(content);
}
std::string ipfs::Loader::UnescapeUrlComponent(std::string_view comp) const {
  using Rule = base::UnescapeRule;
  auto rules = Rule::PATH_SEPARATORS |
               Rule::URL_SPECIAL_CHARS_EXCEPT_PATH_SEPARATORS | Rule::SPACES;
  return base::UnescapeURLComponent({comp.data(), comp.size()}, rules);
}
