#include "loader.h"
#include "inter_request_state.h"

#include "ipfs_client/gateways.h"
#include "ipfs_client/unixfs_path_resolver.h"

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

#include <fstream>

ipfs::Loader::Loader(network::mojom::URLLoaderFactory* handles_http,
                     InterRequestState& state)
    : state_{state},
      lower_loader_factory_{handles_http},
      sched_{state_.gateways().GenerateList(),
             [this](auto&& bg) { this->CreateRequest(std::move(bg)); }} {}
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
  if (resource_request.url.SchemeIs("ipfs")) {
    auto second_slash = path.find_first_of("/?", 5);
    auto cid = path.substr(5, second_slash - 5);
    second_slash = path.find('/', 5);
    std::string remainder{"/"};
    if (second_slash < path.size()) {
      remainder.assign(path.substr(second_slash + 1));
    }
    LOG(ERROR) << "cid=" << cid << " remainder=" << remainder;
    me->resolver_ = std::make_shared<UnixFsPathResolver>(
        me->state_.storage(), cid, remainder,
        [me](auto cid) { me->CreateBlockRequest(cid); },
        [](auto c) { LOG(ERROR) << "TOOD prefetch cid" << c; },
        [me](auto content) { me->partial_block_.append(content); },
        [me](auto mime_type) { me->BlocksComplete(mime_type); });
    me->resolver_->Step(me->resolver_);
  } else {
    me->sched_.Enqueue(path, Scheduler::Priority::Required);
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
void ipfs::Loader::CreateBlockRequest(std::string cid) {
  LOG(INFO) << __PRETTY_FUNCTION__ << " (" << cid << ").";
  sched_.Enqueue("ipfs/" + cid + "?format=raw", Scheduler::Priority::Required);
}
void ipfs::Loader::CreateRequest(BusyGateway&& assigned) {
  auto url_suffix = assigned->current_task();
  auto req = std::make_unique<network::ResourceRequest>();
  auto url = assigned->url();
  LOG(INFO) << "Starting a gateway request: " << url;
  req->url = GURL{url};
  auto idx = gateway_requests_.size();
  gateway_requests_.emplace_back(
      std::move(assigned), network::SimpleURLLoader::Create(
                               std::move(req), kTrafficAnnotation, FROM_HERE));
  auto cb =
      base::BindOnce(&ipfs::Loader::OnGatewayResponse, base::Unretained(this),
                     shared_from_this(), url_suffix, idx);
  // TODO - proper requesting with full features (pause, explict cancel, etc.).
  // May require not using SimpleURLLoader
  gateway_requests_.back()
      .second->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
          lower_loader_factory_, std::move(cb));
}
void ipfs::Loader::OnGatewayResponse(ptr,
                                     std::string,
                                     std::size_t req_idx,
                                     std::unique_ptr<std::string> body) {
  auto& gw = gateway_requests_.at(req_idx).first;
  auto ipfs_path = gw->current_task();
  if (ipfs_path.ends_with("?format=raw")) {
    LOG(INFO) << "Got a response to a block request from " << gw->url();
    if (!body || body->empty()) {
      LOG(ERROR) << "Except the body was empty, boo.";
      gw->failed();
      gw.reset();
      sched_.IssueRequests();
      resolver_->Step(resolver_);
      sched_.IssueRequests();
      return;
    }
    ipfs_path.erase(0, 5);  // ipfs/
    ipfs_path.erase(ipfs_path.find('?'));
    LOG(INFO) << "Storing CID=" << ipfs_path;
    std::ofstream block_file_for_debug{ipfs_path};
    block_file_for_debug.write(body->data(), body->size());
    state_.storage().Store(ipfs_path, Block{*body});
    resolver_->Step(resolver_);
    sched_.IssueRequests();
    return;
  }
  auto& http_loader = gateway_requests_.at(req_idx).second;
  auto prefix =
      gw->url_prefix();  // Doing a deep copy to avoid reference invalidation
                         // concerns below, for simplicity.
  auto url = gw->url();
  if (handle_response(gw.get(), http_loader.get(), body.get())) {
    for (auto& gr : gateway_requests_) {
      if (gr.first && gr.first != gw &&
          gr.first->current_task() == gw->current_task()) {
        gr.first.reset();
        gr.second.reset();
      }
    }
    LOG(INFO) << "Promoting " << prefix << " due to success of " << url;
    state_.gateways().promote(prefix);
  } else {
    gw->failed();
    gw.reset();
    auto failure = sched_.DetectCompleteFailure();
    if (failure.empty()) {
      sched_.IssueRequests();
    } else {
      // Creative reinterpretation of 'gateway' in 'bad gateway'
      client_->OnComplete(network::URLLoaderCompletionStatus{502});
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

  //  client_->OnReceiveResponse(std::move(head_out),
  //                             std::move(pipe_cons_)  // Danger Will
  //                             Robinson!d
  //                             ,
  //                             absl::nullopt);
  //  client_->OnComplete(network::URLLoaderCompletionStatus{});
  return true;
}
void ipfs::Loader::BlocksComplete(std::string mime_type) {
  auto result =
      mojo::CreateDataPipe(partial_block_.size(), pipe_prod_, pipe_cons_);
  if (result) {
    LOG(ERROR) << " ERROR: failed to create data pipe: " << result;
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
}