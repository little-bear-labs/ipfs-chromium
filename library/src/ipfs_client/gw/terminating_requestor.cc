#include "ipfs_client/gw/terminating_requestor.h"

#include <ipfs_client/gw/gateway_request.h>

#include "log_macros.h"

using Self = ipfs::gw::TerminatingRequestor;

std::string_view Self::name() const {
  return "Terminating requestor";
}
auto Self::handle(ipfs::gw::RequestPtr r) -> HandleOutcome {
  if (r->type == Type::Zombie) {
    return HandleOutcome::DONE;
  } else if (r->parallel) {
    return HandleOutcome::PENDING;
  } else if (r->retry_at) {
    LOG(INFO) << "Calling through retry...";
    auto p = r->retry_at;
    r->retry_at.reset();
    p->request(r);
    return HandleOutcome::PENDING;
  } else {
    LOG(ERROR) << "Out of options, giving up on gateway request "
               << r->debug_string();
    definitive_failure(r);
    return HandleOutcome::DONE;
  }
}
