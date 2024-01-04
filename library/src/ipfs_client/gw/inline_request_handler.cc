#include <ipfs_client/gw/inline_request_handler.h>

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipld/chunk.h>
#include <ipfs_client/orchestrator.h>

#include "log_macros.h"

using Self = ipfs::gw::InlineRequestHandler;

std::string_view Self::name() const {
  return "InlineRequestHandler";
}
auto Self::handle(ipfs::gw::RequestPtr req) -> HandleOutcome {
  if (req->type != gw::Type::Identity) {
    return HandleOutcome::NOT_HANDLED;
  }
  std::string data{req->identity_data()};
  LOG(INFO) << "Responding to inline CID without using network.";
  req->RespondSuccessfully(data, api_);
  return HandleOutcome::DONE;
}
