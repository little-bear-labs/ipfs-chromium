#include "ipfs_url_loader.h"

#include "chromium_http.h"
#include "chromium_ipfs_context.h"
#include "inter_request_state.h"

#include "ipfs_client/ctx/default_gateways.h"
#include "ipfs_client/ipfs_request.h"
#include "ipfs_client/ipld/dag_headers.h"

#include "base/debug/stack_trace.h"
#include "base/notreached.h"
#include "base/strings/stringprintf.h"
#include "base/task/bind_post_task.h"
#include "base/task/thread_pool.h"
#include "base/threading/platform_thread.h"
#include "content/public/browser/browser_thread.h"
#include "net/http/http_status_code.h"
#include "services/network/public/cpp/parsed_headers.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "services/network/url_loader_factory.h"

#include <fstream>

ipfs::IpfsUrlLoader::IpfsUrlLoader(
    network::mojom::URLLoaderFactory& handles_http,
    InterRequestState& state)
    : state_{state}, lower_loader_factory_{handles_http}, api_{state_->api()} {}
ipfs::IpfsUrlLoader::~IpfsUrlLoader() noexcept {
  if (stepper_) {
    stepper_->Stop();
    stepper_.reset();
  }
  if (!complete_) {
    VLOG(1) << "Premature IPFS URLLoader dtor, uri was '" << original_url_
            << "' " << base::debug::StackTrace();
  }
}

void ipfs::IpfsUrlLoader::FollowRedirect(
    std::vector<std::string> const& /* removed_headers */,
    net::HttpRequestHeaders const& /* modified_headers */,
    net::HttpRequestHeaders const& /* modified_cors_exempt_headers */,
    VirtualOptional<::GURL> const& /*new_url*/
) {
  NOTIMPLEMENTED();
}

void ipfs::IpfsUrlLoader::SetPriority(net::RequestPriority priority,
                                      int32_t intra_prio_val) {
  VLOG(2) << "TODO SetPriority(" << priority << ',' << intra_prio_val << ')';
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
  if (resource_request.url.SchemeIs("ipfs") ||
      resource_request.url.SchemeIs("ipns")) {
    auto ns = resource_request.url.scheme();
    auto cid_str = resource_request.url.host();
    auto path = resource_request.url.path();
    auto abs_path = "/" + ns + "/" + cid_str + path;
    me->root_ = cid_str;
    me->api_->with(
        std::make_unique<ChromiumHttp>(*(me->lower_loader_factory_)));
    auto whendone = [me](IpfsRequest const& req, ipfs::Response const& res) {
      if (!res.body_.empty()) {
        me->ReceiveBlockBytes(res.body_);
      }
      me->status_ = res.status_;
      me->resp_loc_ = res.location_;
      if (res.status_ == Response::IMMUTABLY_GONE.status_) {
        auto p = req.path();
        p.pop();
        std::string cid{p.pop()};
        me->DoesNotExist(cid, p.to_string());
      } else {
        me->BlocksComplete(res.mime_, res.headers_);
      }
      DCHECK(me->complete_);
    };
    me->ipfs_request_ = std::make_shared<IpfsRequest>(abs_path, whendone);
    me->stepper_ = std::make_unique<base::RepeatingTimer>();
    //    auto f = [](std::shared_ptr<IpfsUrlLoader> m) { m->TakeStep(); };
    //    auto cb = base::BindOnce(f, me);
    //    content::GetIOThreadTaskRunner({})->PostTask(FROM_HERE,
    //    std::move(cb));
    //    base::ThreadPool::PostTask(FROM_HERE, std::move(cb));
    me->stepper_->Start(FROM_HERE, base::Seconds(2), me.get(),
                        &ipfs::IpfsUrlLoader::TakeStep);
    me->TakeStep();
  } else {
    LOG(ERROR) << "Wrong scheme: " << resource_request.url.scheme();
  }
}

void ipfs::IpfsUrlLoader::OverrideUrl(GURL u) {
  original_url_ = u.spec();
}

void ipfs::IpfsUrlLoader::BlocksComplete(std::string mime_type,
                                         ipld::DagHeaders const& hdrs) {
  VLOG(2) << "Resolved from unix-fs dag a file of type: " << mime_type
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
  complete_ = true;
  auto head = network::mojom::URLResponseHead::New();
  if (mime_type.size()) {
    head->mime_type = mime_type;
  }
  std::uint32_t byte_count = partial_block_.size();
  VLOG(2) << "Calling WriteData(" << byte_count << ")";
  pipe_prod_->WriteData(partial_block_.data(), &byte_count,
                        MOJO_BEGIN_WRITE_DATA_FLAG_ALL_OR_NONE);
  VLOG(2) << "Called WriteData(" << byte_count << ")";
  head->content_length = byte_count;
  head->headers =
      net::HttpResponseHeaders::TryToCreate("access-control-allow-origin: *");
  if (!head->headers) {
    LOG(ERROR) << "\n\tFailed to create headers!\n";
    return;
  }
  auto* reason =
      net::GetHttpReasonPhrase(static_cast<net::HttpStatusCode>(status_));
  auto status_line = base::StringPrintf("HTTP/1.1 %d %s", status_, reason);
  VLOG(2) << "Returning with status line '" << status_line << "'.\n";
  head->headers->ReplaceStatusLine(status_line);
  if (mime_type.size()) {
    head->headers->SetHeader("Content-Type", mime_type);
  }
  head->headers->SetHeader("Access-Control-Allow-Origin", "*");
  head->was_fetched_via_spdy = false;

  for (auto& [n, v] : hdrs.headers()) {
    head->headers->AddHeader(n, v);
  }
  if (resp_loc_.size()) {
    head->headers->AddHeader("Location", resp_loc_);
    LOG(INFO) << "Sending response for " << original_url_ << " with mime type "
              << head->mime_type << " and status line '" << status_line
              << "' @location '" << resp_loc_ << "'";
  } else {
    VLOG(2) << "Sending response for " << original_url_ << " with mime type "
            << head->mime_type << " and status line '" << status_line
              << "' with no location header.";
  }
  head->parsed_headers =
      network::PopulateParsedHeaders(head->headers.get(), GURL{original_url_});
  if (status_ / 100 == 3 && resp_loc_.size()) {
    auto ri = net::RedirectInfo::ComputeRedirectInfo(
        "GET", GURL{original_url_}, net::SiteForCookies{},
        net::RedirectInfo::FirstPartyURLPolicy::UPDATE_URL_ON_REDIRECT,
        net::ReferrerPolicy::NO_REFERRER, "", status_, GURL{resp_loc_},
        std::nullopt, false);
    client_->OnReceiveRedirect(ri, std::move(head));
  } else {
    client_->OnReceiveResponse(std::move(head), std::move(pipe_cons_),
                               absl::nullopt);
  }
  client_->OnComplete(network::URLLoaderCompletionStatus{});
  if (stepper_) {
    stepper_->Stop();
    stepper_.reset();
  }
}
void ipfs::IpfsUrlLoader::DoesNotExist(std::string_view cid,
                                       std::string_view path) {
  VLOG(2) << "Immutable data 404 for " << cid << '/' << path;
  complete_ = true;
  client_->OnComplete(
      network::URLLoaderCompletionStatus{net::ERR_FILE_NOT_FOUND});
  if (stepper_) {
    stepper_->Stop();
    stepper_.reset();
  }
}
void ipfs::IpfsUrlLoader::ReceiveBlockBytes(std::string_view content) {
  partial_block_.append(content);
}
void ipfs::IpfsUrlLoader::TakeStep() {
  if (ipfs_request_) {
    state_->orchestrator().build_response(ipfs_request_);
  }
}
