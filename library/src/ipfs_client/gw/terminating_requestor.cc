#include "ipfs_client/gw/terminating_requestor.h"

#include <ipfs_client/gw/gateway_request.h>

#include "ipfs_client/gw/gateway_request_type.h"
#include "log_macros.h"

using Self = ipfs::gw::TerminatingRequestor;

std::string_view Self::name() const {
  return "Terminating requestor";
}
auto Self::handle(ipfs::gw::RequestPtr r) -> HandleOutcome {
  if (r->Finished()) {
    return HandleOutcome::DONE;
  } else if (r->parallel) {
    return HandleOutcome::PENDING;
  } else {
    LOG(ERROR) << "Out of options, giving up on gateway request "
               << r->debug_string();
    definitive_failure(r);
    r->type = GatewayRequestType::Zombie;
    return HandleOutcome::DONE;
  }
}
