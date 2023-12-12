#include <ipfs_client/gw/requestor.h>

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipld/dag_node.h>

#include <ipfs_client/dag_block.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/orchestrator.h>
#include <ipfs_client/response.h>

#include "log_macros.h"

using Self = ipfs::gw::Requestor;
using ReqPtr = std::shared_ptr<ipfs::gw::GatewayRequest>;

Self& Self::or_else(std::shared_ptr<Self> p) {
  if (next_) {
    next_->or_else(p);
  } else {
    VLOG(2) << name() << " is followed by " << p->name();
    next_ = p;
  }
  if (api_ && !p->api_) {
    VLOG(1) << name() << " granting context to " << p->name();
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
void Self::failure(ipfs::gw::RequestPtr r) const {
  if (next_) {
    LOG(WARNING) << name() << " failed on " << r->debug_string()
                 << " ... passing along to " << next_->name();
    next_->request(r);
  } else {
    definitive_failure(r);
  }
}
void Self::definitive_failure(ipfs::gw::RequestPtr r) const {
  DCHECK(r);
  DCHECK(r->dependent);
  r->dependent->finish(Response::PLAIN_NOT_FOUND);
}
void Self::success(ipfs::gw::RequestPtr req, std::string_view body) const {
  Response res;
  res.status_ = 200;
  res.body_.assign(body);
  receive_response(req, res);
}
void Self::iterate_nodes(
    GatewayRequest const& req,
    Response const& res,
    std::function<void(std::string, ipld::NodePtr)> cb) const {
  if (res.body_.empty()) {
    return;
  }
  Cid cid(req.main_param);
  if (!cid.valid()) {
    return;
  }
  Block b{std::move(cid), res.body_};
  if (!b.valid()) {
    return;
  }
  auto n = ipld::DagNode::fromBlock(b);
  if (n) {
    cb(req.main_param, n);
  }
}
void Self::receive_response(ipfs::gw::RequestPtr req,
                            ipfs::Response const& res) const {
  if (res.status_ / 100 == 2) {
    req->RespondSuccessfully(res.body_, api_);
  } else if (req->parallel == 0) {
    LOG(ERROR) << "Finally failing on " << req->debug_string();
    definitive_failure(req);
  }
}
void Self::forward(ipfs::gw::RequestPtr req) const {
  if (next_) {
    next_->request(req);
  }
}
void Self::api(std::shared_ptr<ContextApi> a) {
  api_ = a;
}