#include "requestor_pool.h"

#include <ipfs_client/gw/gateway_request.h>

#include "log_macros.h"

using Self = ipfs::gw::RequestorPool;

std::string_view Self::name() const {
  return "requestor pool";
}
Self& Self::add(std::shared_ptr<Requestor> r) {
  if (api_ && !(r->api_)) {
    r->api_ = api_;
  }
  pool_.push_back(r);
  r->or_else(shared_from_this());
  return *this;
}
auto Self::handle(ipfs::gw::RequestPtr req) -> HandleOutcome {
  auto now = std::time(nullptr);
  for (auto i = 0UL; i * 2 < waiting_.size(); ++i) {
    auto& t = waiting_.front().when;
    if (t != now) {
      auto to_pop = waiting_.front();
      waiting_.pop();
      check(to_pop);
    }
  }
  return check({req, 0UL, 0L});
}
auto Self::check(Waiting w) -> HandleOutcome {
  using O = HandleOutcome;
  auto next_retry = pool_.size();
  auto req = w.req;
  if (req->PartiallyRedundant()) {
    return O::DONE;
  }
  for (auto i = w.at_idx; i < pool_.size(); ++i) {
    if (req->type == Type::Zombie) {
      return O::DONE;
    }
    auto& tor = pool_[i];
    switch (tor->handle(req)) {
      case O::DONE:
        LOG(INFO) << "RequestorPool::handle returning DONE because a member of "
                     "the pool's handle returned DONE.";
        return O::DONE;
      case O::PENDING:
      case O::PARALLEL:
        req->parallel++;
        break;
      case O::MAYBE_LATER:
        if (next_retry == pool_.size()) {
          next_retry = i;
        }
        break;
      case O::NOT_HANDLED:
        break;
    }
  }
  if (req->parallel > 0) {
    return O::PENDING;
  }
  if (next_retry < pool_.size()) {
    w.when = std::time(nullptr);
    waiting_.emplace(w);
    return O::PENDING;
  }
  VLOG(1) << "Have exhausted all requestors in pool looking for "
          << req->debug_string();
  return O::NOT_HANDLED;
}
