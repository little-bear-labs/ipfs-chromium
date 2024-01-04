#include <ipfs_client/gw/requestor.h>

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipld/dag_node.h>

#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/orchestrator.h>
#include <ipfs_client/pb_dag.h>
#include <ipfs_client/response.h>

#include "log_macros.h"

using Self = ipfs::gw::Requestor;
using ReqPtr = std::shared_ptr<ipfs::gw::GatewayRequest>;

Self& Self::or_else(std::shared_ptr<Self> p) {
  if (next_) {
    next_->or_else(p);
  } else {
    next_ = p;
  }
  if (api_ && !p->api_) {
    VLOG(2) << name() << " granting context to " << p->name();
    p->api_ = api_;
  }
  return *this;
}

void Self::request(ReqPtr req) {
  if (!req || req->type == Type::Zombie) {
    return;
  }
  switch (handle(req)) {
    case HandleOutcome::MAYBE_LATER:
      // TODO
      forward(req);
      break;
    case HandleOutcome::PARALLEL:
    case HandleOutcome::NOT_HANDLED:
      if (next_) {
        next_->request(req);
      } else {
        LOG(ERROR) << "Ran out of Requestors in the chain while looking for "
                      "one that can handle "
                   << req->debug_string();
        definitive_failure(req);
      }
      break;
    case HandleOutcome::PENDING:
      break;
    case HandleOutcome::DONE:
      VLOG(2) << req->debug_string() << " finished synchronously: " << name();
      break;
  }
}
void Self::definitive_failure(ipfs::gw::RequestPtr r) const {
  DCHECK(r);
  DCHECK(r->dependent);
  r->dependent->finish(Response::PLAIN_NOT_FOUND);
}

void Self::forward(ipfs::gw::RequestPtr req) const {
  if (next_) {
    next_->request(req);
  }
}
void Self::api(std::shared_ptr<ContextApi> a) {
  api_ = a;
}