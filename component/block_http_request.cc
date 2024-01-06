#include "block_http_request.h"

#include <services/network/public/cpp/resource_request.h>
#include <services/network/public/cpp/simple_url_loader.h>
#include <services/network/public/mojom/url_response_head.mojom.h>

using Self = ipfs::BlockHttpRequest;

namespace {
constexpr net::NetworkTrafficAnnotationTag kTrafficAnnotation =
    net::DefineNetworkTrafficAnnotation("ipfs_gateway_request", R"(
      semantics {
        sender: "IPFS component"
        description:
          "Sends a request to an IPFS gateway."
        trigger:
          "Processing of an ipfs:// or ipns:// URL."
        data: "None"
        destination: WEBSITE
      }
      policy {
        cookies_allowed: NO
        setting: "EnableIpfs"
      }
    )");
}

Self::BlockHttpRequest(ipfs::HttpRequestDescription req_inf,
                       HttpCompleteCallback cb)
    : inf_{req_inf}, callback_{cb} {}
Self::~BlockHttpRequest() noexcept {}

void Self::send(raw_ptr<network::mojom::URLLoaderFactory> loader_factory) {
  auto req = std::make_unique<network::ResourceRequest>();
  req->url = GURL{inf_.url};
  req->priority = net::HIGHEST;  // TODO
  if (!inf_.accept.empty()) {
    req->headers.SetHeader("Accept", inf_.accept);
  }
  using L = network::SimpleURLLoader;
  loader_ = L::Create(std::move(req), kTrafficAnnotation, FROM_HERE);
  loader_->SetTimeoutDuration(base::Seconds(inf_.timeout_seconds));
  loader_->SetAllowHttpErrorResults(true);
  loader_->SetOnResponseStartedCallback(
      base::BindOnce(&Self::OnResponseHead, base::Unretained(this)));
  auto bound = base::BindOnce(&Self::OnResponse, base::Unretained(this),
                              shared_from_this());
  DCHECK(loader_factory);
  if (auto sz = inf_.max_response_size) {
    loader_->DownloadToString(loader_factory, std::move(bound), sz.value());
  } else {
    loader_->DownloadToStringOfUnboundedSizeUntilCrashAndDie(loader_factory,
                                                             std::move(bound));
  }
}
void Self::OnResponse(std::shared_ptr<Self>,
                      std::unique_ptr<std::string> body) {
  DCHECK(loader_);
  int status;
  switch (loader_->NetError()) {
    case net::Error::OK:
      status = 200;
      break;
    case net::Error::ERR_TIMED_OUT:
      status = 408;
      break;
    default:
      //      VLOG(2) << "NetErr " << loader_->NetError() << " for " <<
      //      inf_.url;
      status = 500;
  }
  //  auto sz = body ? body->size() : 0UL;
  auto const* head = loader_->ResponseInfo();
  if (head) {
    OnResponseHead({}, *head);
  }
  auto sp = status_line_.find(' ');
  if (sp < status_line_.size()) {
    status = std::atoi(status_line_.c_str() + sp + 1);
  }
  if (body) {
    callback_(status, *body, header_accessor_);
  } else {
    callback_(status, "", header_accessor_);
  }
}
void Self::OnResponseHead(
    GURL const&,
    network::mojom::URLResponseHead const& response_head) {
  if (!response_head.headers) {
    return;
  }
  auto head = response_head.headers;
  status_line_ = head->GetStatusLine();
  header_accessor_ = [head](std::string_view k) {
    std::string val;
    head->EnumerateHeader(nullptr, k, &val);
    return val;
  };
}
