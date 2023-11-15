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
    VLOG(2) << inf_.url << " has Accept header " << inf_.accept;
    req->headers.SetHeader("Accept", inf_.accept);
  }
  using L = network::SimpleURLLoader;
  loader_ = L::Create(std::move(req), kTrafficAnnotation, FROM_HERE);
  loader_->SetTimeoutDuration(base::Seconds(inf_.timeout_seconds));
  loader_->SetAllowHttpErrorResults(true);
  auto bound = base::BindOnce(&Self::OnResponse, base::Unretained(this),
                              shared_from_this());
  DCHECK(loader_factory);
  loader_->DownloadToString(loader_factory, std::move(bound),
                            gw::BLOCK_RESPONSE_BUFFER_SIZE);
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
      status = 500;
  }
  ContextApi::HeaderAccess header_accessor = [](auto) { return std::string{}; };
  auto sz = body ? body->size() : 0UL;
  if (loader_->NetError() == net::Error::ERR_TIMED_OUT) {
    VLOG(1) << "HTTP request timed out: " << inf_.url << " after "
            << inf_.timeout_seconds << "s.";
  } else if (loader_->NetError()) {
    LOG(INFO) << "NetErr " << loader_->NetError() << " for " << inf_.url;
  }
  VLOG(1) << "Handling HTTP response body of size " << sz << " with NetErr "
          << loader_->NetError() << " for " << inf_.url;
  auto const* head = loader_->ResponseInfo();
  if (head) {
    DCHECK(head->headers);
    auto status_line = head->headers->GetStatusLine();
    auto sp = status_line.find(' ');
    if (sp < status_line.size()) {
      VLOG(1) << "HTTP response status='" << status_line << "'.";
      status = std::atoi(status_line.c_str() + sp + 1);
    } else {
      LOG(WARNING) << "Status line malformed.";
    }
    header_accessor = [head](std::string_view k) {
      std::string val;
      head->headers->EnumerateHeader(nullptr, k, &val);
      return val;
    };
  } else {
    VLOG(1) << "No response header info?";
  }
  if (body) {
    callback_(status, *body, header_accessor);
  } else {
    callback_(status, "", header_accessor);
  }
}
