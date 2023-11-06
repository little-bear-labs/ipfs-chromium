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
  pool.push_back(r);
  r->or_else(shared_from_this());
  return *this;
}
auto Self::handle(ipfs::gw::RequestPtr req) -> HandleOutcome {
  for (auto i = 0UL; i * 2 < waiting.size(); ++i) {
    auto popped = waiting.front();
    check(popped.first, popped.second);
  }
  return check(req, 0UL);
}
auto Self::check(ipfs::gw::RequestPtr req, std::size_t start) -> HandleOutcome {
  using O = HandleOutcome;
  if (req->type == Type::Zombie) {
    return O::DONE;
  }
  auto next_retry = pool.size();
  for (auto i = start; i < pool.size(); ++i) {
    auto& tor = pool[i];
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
        if (next_retry == pool.size()) {
          next_retry = i;
        }
        break;
      case O::NOT_HANDLED:
        break;
    }
  }
  if (req->parallel > 0) {
    LOG(INFO) << req->parallel << " requestors have picked up the task.";
    return O::PENDING;
  }
  if (next_retry < pool.size()) {
    LOG(INFO) << "No requestors are available for " << req->debug_string()
              << " right now, will retry at index " << next_retry;
    waiting.emplace(req, next_retry);
    return O::PENDING;
  }
  LOG(WARNING) << "Have exhausted all requestors in pool looking for "
               << req->debug_string();
  return O::NOT_HANDLED;
}
