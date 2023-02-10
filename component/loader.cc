#include "loader.h"
#include "inter_request_state.h"

#include "ipfs_client/gateways.h"

#include "base/notreached.h"
#include "base/strings/escape.h"
#include "base/threading/platform_thread.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "ipfs_client/ipfs_uri.h"
#include "services/network/public/cpp/parsed_headers.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "services/network/url_loader_factory.h"

ipfs::Loader::Loader(network::mojom::URLLoaderFactory* handles_http,
                     InterRequestState& state)
    : state_{state},
      lower_loader_factory_{handles_http},
      sched_{state_.gateways().GenerateList()} {}
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
void ipfs::Loader::SetPriority(net::RequestPriority /*priority*/,
                               int32_t /*intra_priority_value*/) {
  NOTIMPLEMENTED();
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
  std::clog << __PRETTY_FUNCTION__ << " got " << resource_request.url.spec()
            << std::endl;
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
    path = IpfsOverHttpUrl2IpfsGatewayPath(resource_request.url.spec());
  }
  me->startup(me, path, 10);
}

void ipfs::Loader::startup(ptr me,
                           std::string requested_path,
                           unsigned concurrent) {
  while (concurrent > 0 && start_gateway_request(me, requested_path)) {
    --concurrent;
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
bool ipfs::Loader::start_gateway_request(ptr me, std::string requested_path) {
  auto [status, assigned] = sched_.schedule(requested_path);
  if (status == Scheduler::Result::Failed) {
    LOG(ERROR) << "Request completely failed: " << requested_path;
    return false;
  }
  if (!assigned) {
    DCHECK(status == Scheduler::Result::InProgress);
    return true;
  }
  // TODO lots of things wrong with this function
  auto req = std::make_unique<network::ResourceRequest>();
  auto url = assigned->url();
  LOG(INFO) << "Starting a gateway request: " << url;
  req->url = GURL{url};
  auto idx = gateway_requests_.size();
  gateway_requests_.emplace_back(
      std::move(assigned), network::SimpleURLLoader::Create(
                               std::move(req), kTrafficAnnotation, FROM_HERE));
  auto cb = base::BindOnce(&ipfs::Loader::on_gateway_response,
                           base::Unretained(this), me, requested_path, idx);
  gateway_requests_.back()
      .second->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
          lower_loader_factory_, std::move(cb));
  return true;
}
void ipfs::Loader::on_gateway_response(ptr me,
                                       std::string requested_path,
                                       std::size_t req_idx,
                                       std::unique_ptr<std::string> body) {
  auto& gw = gateway_requests_.at(req_idx).first;
  auto& http_loader = gateway_requests_.at(req_idx).second;
  auto prefix =
      gw->url_prefix();  // Doing a deep copy to avoid reference invalidation
                         // concerns below, for simplicity.
  auto url = gw->url();
  if (handle_response(gw.get(), http_loader.get(), body.get())) {
    std::iter_swap(gateway_requests_.begin(),
                   gateway_requests_.begin() + req_idx);
    // TODO remove only identical requests
    gateway_requests_.erase(std::next(gateway_requests_.begin()),
                            gateway_requests_.end());
    LOG(INFO) << "Promoting " << prefix << " due to success of " << url;
    state_.gateways().promote(prefix);
  } else {
    gw->failed();
    gw.reset();
    if (start_gateway_request(me, requested_path)) {
      LOG(INFO) << "Response failure for " << url << ", starting new request.";
    } else if (http_loader->CompletionStatus().has_value()) {
      LOG(ERROR) << "Complete failure on " << requested_path << '\n';
      client_->OnComplete(http_loader->CompletionStatus().value());
    }
    LOG(INFO) << "Demoting " << prefix;
    state_.gateways().demote(prefix);
    // TODO http_loader.reset();
  }
}
bool ipfs::Loader::handle_response(Gateway* gw,
                                   network::SimpleURLLoader* gw_req,
                                   std::string* body) {
  if (!body) {
    LOG(INFO) << "handle_response(" << gw->url()
              << ") Null body - presumably http error.\n";
    return false;
  }
  network::mojom::URLResponseHead const* head = gw_req->ResponseInfo();
  if (!head) {
    LOG(INFO) << "handle_response(" << gw->url() << ") Null head.\n";
    return false;
  }
  std::string reported_path;
  head->headers->EnumerateHeader(nullptr, "X-Ipfs-Path", &reported_path);
  if (reported_path.empty()) {
    LOG(INFO) << "handle_response(" << gw->url()
              << ") gave us a non-ipfs response.\n";
    return false;
  }
  if (gw->url().find(reported_path) == std::string::npos) {
    LOG(INFO) << "Requested " << gw->url() << " but got a response to "
              << reported_path
              << ". Most likely this is just a different hash algo.\n";
  }
  auto result = mojo::CreateDataPipe(body->size(), pipe_prod_, pipe_cons_);
  if (result) {
    LOG(ERROR) << " ERROR: failed to create data pipe: " << result;
    return false;
  }
  std::uint32_t write_size = body->size();
  pipe_prod_->WriteData(body->c_str(), &write_size,
                        MOJO_BEGIN_WRITE_DATA_FLAG_ALL_OR_NONE);

  auto head_out = head->Clone();

  head_out->mime_type = head->mime_type;  //"text/html";
  head_out->content_length = write_size;
  head_out->headers->SetHeader("access-control-allow-origin", "*");
  head_out->headers->RemoveHeader("content-security-policy");
  head_out->headers->RemoveHeader("referrer-policy");
  head_out->headers->RemoveHeader("link");
  //    auto raw = head_out->headers->raw_headers();
  //    std::replace(raw.begin(),raw.end(),'\0','\n');
  //    std::clog << "headers:|" << raw << "|\n";
  if (original_url_.ends_with("%2F")) {
    original_url_.append("index.html");  // TODO This isn't quite right.
  }
  head_out->parsed_headers =
      network::PopulateParsedHeaders(head->headers.get(), GURL{original_url_});
  /*
  std::clog << "ipfs_uri=" << ipfs_uri << " ... CSP: \n";
  for ( auto& csp : head_out->parsed_headers->content_security_policy ) {
      if ( csp ) {
          std::clog << "  " << csp->header->header_value << '\n';
      } else {
          std::clog << "  null\n";
      }
  }
   */
  head_out->was_fetched_via_spdy = false;

  client_->OnReceiveResponse(std::move(head_out),
                             std::move(pipe_cons_)  // Danger Will Robinson!d
                             ,
                             absl::nullopt);
  client_->OnComplete(network::URLLoaderCompletionStatus{});
  return true;
}
