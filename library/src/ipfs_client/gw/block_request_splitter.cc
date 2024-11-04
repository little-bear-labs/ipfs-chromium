#include <ipfs_client/gw/block_request_splitter.h>

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/gw/gateway_request_type.h>

#include "ipfs_client/gw/requestor.h"
#include "log_macros.h"

#include <algorithm>
#include <string_view>
#include <memory>

using Self = ipfs::gw::BlockRequestSplitter;

auto Self::name() const -> std::string_view {
  return "BlockRequestSplitter";
}
auto Self::handle(ipfs::gw::RequestPtr req) -> HandleOutcome {
  if (req->type != GatewayRequestType::Car) {
    return HandleOutcome::NOT_HANDLED;
  }
  {
    auto br = std::make_shared<gw::GatewayRequest>(*req);
    br->type = GatewayRequestType::Block;
    br->path.clear();
    forward(br);
  }
  if (api_ && (api_->gw_cfg().RoutingApiDiscoveryDefaultRate() != 0U)) {
    auto it = std::find(
        recent_provider_requests.begin(),
        recent_provider_requests.end(),
        req->affinity);
    if (recent_provider_requests.end() == it) {
      auto i = old_provider_request % recent_provider_requests.size();
      recent_provider_requests[i] = req->affinity;
      ++old_provider_request;
      auto pr = std::make_shared<gw::GatewayRequest>(*req);
      pr->type = GatewayRequestType::Providers;
      pr->path.clear();
      pr->affinity.clear();
      forward(pr);
    }
  } else if (++old_provider_request == 1UL) {
    LOG(INFO) << "Routing requests disabled.";
  }
  return HandleOutcome::NOT_HANDLED;
}
