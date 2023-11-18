#include <ipfs_client/gw/inline_request_handler.h>

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipld/chunk.h>
#include <ipfs_client/orchestrator.h>

using Self = ipfs::gw::InlineRequestHandler;

std::string_view Self::name() const {
  return "InlineRequestHandler";
}
auto Self::handle(ipfs::gw::RequestPtr req) -> HandleOutcome {
  if (req->type != gw::Type::Identity) {
    return HandleOutcome::NOT_HANDLED;
  }
  std::string data{req->identity_data()};
  req->RespondSuccessfully(data, api_);
  return HandleOutcome::DONE;
}
