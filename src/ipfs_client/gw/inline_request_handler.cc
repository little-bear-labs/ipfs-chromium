#include <ipfs_client/gw/inline_request_handler.h>

#include <ipfs_client/gw/gateway_request_type.h>
#include <ipfs_client/gw/requestor.h>
#include <ipfs_client/gw/gateway_request.h>

#include <string_view>
#include <string>

#include "log_macros.h"

using Self = ipfs::gw::InlineRequestHandler;

auto Self::name() const -> std::string_view {
  return "InlineRequestHandler";
}
auto Self::handle(RequestPtr request) -> HandleOutcome {
  GatewayRequest& req = *request;
  if (req.type != GatewayRequestType::Identity) {
    return HandleOutcome::NOT_HANDLED;
  }
  std::string const data{req.identity_data()};
  VLOG(2) << "Responding to inline CID without using network.";
  req.RespondSuccessfully(data, api_, {});
  return HandleOutcome::DONE;
}
