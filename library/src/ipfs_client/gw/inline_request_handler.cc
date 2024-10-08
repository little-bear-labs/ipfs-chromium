#include <ipfs_client/gw/inline_request_handler.h>

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipld/chunk.h>
#include <ipfs_client/partition.h>

#include "ipfs_client/gw/gateway_request_type.h"
#include "log_macros.h"

using Self = ipfs::gw::InlineRequestHandler;

std::string_view Self::name() const {
  return "InlineRequestHandler";
}
auto Self::handle(ipfs::gw::RequestPtr req) -> HandleOutcome {
  if (req->type != GatewayRequestType::Identity) {
    return HandleOutcome::NOT_HANDLED;
  }
  std::string data{req->identity_data()};
  VLOG(2) << "Responding to inline CID without using network.";
  req->RespondSuccessfully(data, api_, {});
  return HandleOutcome::DONE;
}
