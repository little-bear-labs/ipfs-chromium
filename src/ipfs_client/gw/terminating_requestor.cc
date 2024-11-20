#include "ipfs_client/gw/terminating_requestor.h"

#include <ipfs_client/gw/gateway_request_type.h>
#include <ipfs_client/gw/requestor.h>
#include <ipfs_client/gw/gateway_request.h>

#include "log_macros.h"

#include <string_view>

using Self = ipfs::gw::TerminatingRequestor;

auto Self::name() const -> std::string_view {
  return "Terminating requestor";
}
auto Self::handle(RequestPtr reqp) -> HandleOutcome {
  GatewayRequest& req = *reqp;
  if (req.Finished()) {
    return HandleOutcome::DONE;
  } else if (req.parallel != 0) {
    return HandleOutcome::PENDING;
  } else {
    LOG(ERROR) << "Out of options, giving up on gateway request "
               << req.debug_string();
    definitive_failure(reqp);
    req.type = GatewayRequestType::Zombie;
    return HandleOutcome::DONE;
  }
}
