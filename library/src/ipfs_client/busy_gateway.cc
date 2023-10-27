#include <ipfs_client/busy_gateway.h>
#include <ipfs_client/context_api.h>
#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/response.h>

#include "log_macros.h"

#include <ipfs_client/scheduler.h>

ipfs::BusyGateway::BusyGateway(std::string pre, UrlSpec spec, Scheduler* sched)
    : prefix_(pre), spec_(spec), scheduler_{sched}, maybe_offset_{0UL} {}
ipfs::BusyGateway::BusyGateway(BusyGateway&& rhs)
    : prefix_(rhs.prefix_),
      spec_(rhs.spec_),
      scheduler_(rhs.scheduler_),
      maybe_offset_(0UL) {
  //  LOG(INFO) << "BusyGateway<mov ctor>(" << prefix_ << ',' << suffix_ << ')';
  rhs.prefix_.clear();
  rhs.spec_ = {};
  rhs.scheduler_ = nullptr;
  rhs.maybe_offset_ = 0UL;
  srcreq = rhs.srcreq;
}
ipfs::BusyGateway::~BusyGateway() {
  if (*this && get()) {
    (*this)->TaskCancelled(spec_);
  }
}
ipfs::Gateway* ipfs::BusyGateway::get() {
  if (!scheduler_ || prefix_.empty() || spec_.none()) {
    return nullptr;
  }
  auto& gws = scheduler_->gateways_;
  std::string pre = prefix_;
  auto match = [pre](auto& gw) { return gw.url_prefix() == pre; };
  auto found = std::find_if(gws.begin() + maybe_offset_, gws.end(), match);
  if (gws.end() != found) {
    maybe_offset_ = std::distance(gws.begin(), gws.end());
    return &*found;
  } else if (maybe_offset_) {
    maybe_offset_ = 0UL;
    return get();
  } else {
    return nullptr;
  }
}
ipfs::Gateway const* ipfs::BusyGateway::operator->() const {
  return const_cast<BusyGateway*>(this)->get();
}
ipfs::Gateway* ipfs::BusyGateway::operator->() {
  return get();
}
ipfs::Gateway& ipfs::BusyGateway::operator*() {
  DCHECK(get());
  return *get();
}
ipfs::BusyGateway::operator bool() const {
  return scheduler_ && prefix_.size() && !spec_.none();
}
void ipfs::BusyGateway::reset() {
  //  LOG(INFO) << "BusyGateway::reset()";
  if (scheduler_) {
    auto todo_it = scheduler_->task2todo_.find(spec_);
    if (todo_it != scheduler_->task2todo_.end()) {
      std::set<std::shared_ptr<GatewayRequest>>& reqs =
          todo_it->second.requests;
      auto e = std::find_if(reqs.begin(), reqs.end(), [this](auto& r) {
        return r->gateway.get() == get();
      });
      if (reqs.end() != e) {
        reqs.erase(e);
      }
      //      scheduler_->UpdateDevPage();
    }
    scheduler_ = nullptr;
  }
  prefix_.clear();
  spec_ = {};
}
void ipfs::BusyGateway::Success(Gateways& g, std::shared_ptr<ContextApi> api) {
  if (prefix_.empty()) {
    return;
  }
  g.promote(prefix_);
  DCHECK(get());
  get()->TaskSuccess(spec_);
  auto sched = scheduler_;
  sched->TaskComplete(spec_);
  if (maybe_offset_) {
    sched->CheckSwap(--maybe_offset_);
  }
  reset();
  sched->IssueRequests(api);
}
void ipfs::BusyGateway::Failure(Gateways& g, std::shared_ptr<ContextApi> api) {
  DCHECK(prefix_.size() > 0U);
  g.demote(prefix_);
  get()->TaskFailed(spec_);
  auto sched = scheduler_;
  sched->CheckSwap(maybe_offset_);
  if (sched->DetectCompleteFailure(spec_)) {
    LOG(WARNING) << "Giving up on task " << spec_.suffix
                 << " due to complete failure.";
    if (srcreq) {
      srcreq->dependent->finish(Response::PLAIN_NOT_FOUND);
    }
    sched->TaskComplete(spec_);
  } else {
    LOG(INFO) << prefix_ << " gave up on " << spec_.suffix
              << " , but maybe others may continue.";
    sched->IssueRequests(api);
  }
  reset();
  sched->IssueRequests(api);
}
