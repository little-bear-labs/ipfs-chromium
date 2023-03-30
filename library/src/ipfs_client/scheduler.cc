#include <ipfs_client/framework_api.h>
#include <ipfs_client/scheduler.h>

#include <vocab/log_macros.h>

#include <cassert>

#include <algorithm>
#include <iostream>

ipfs::Scheduler::Scheduler(GatewayList&& initial_list,
                           unsigned max_conc,
                           unsigned max_dup)
    : gateways_{initial_list}, max_conc_(max_conc), max_dup_(max_dup) {}

ipfs::Scheduler::~Scheduler() {}

void ipfs::Scheduler::Enqueue(std::shared_ptr<FrameworkApi> api,
                              std::shared_ptr<DagListener> listener,
                              std::string const& suffix,
                              Priority p) {
  auto& todos = todos_.at(static_cast<unsigned>(p));
  auto pred = [suffix](auto& t) { return t.suffix == suffix; };
  auto it = std::find_if(todos.begin(), todos.end(), pred);
  if (todos.end() == it) {
    todos.emplace_back(suffix);
    it = std::prev(todos.end());
  }
  if (listener) {
    it->listeners.insert(listener);
  }
  IssueRequests(api, listener);
}
void ipfs::Scheduler::IssueRequests(std::shared_ptr<FrameworkApi> api,
                                    std::shared_ptr<DagListener>& listener) {
  Issue(api, listener, todos_.at(0), 0);
  if (ongoing_ >= max_conc_) {
    auto t = std::time(nullptr);
    if (!fudge_) {
      fudge_ = t;
      return;
    } else if (t - fudge_ < 9) {
      return;
    }
    LOG(INFO) << "Overtime " << ongoing_ << " " << max_conc_ << " "
              << (t - fudge_);
  }
  fudge_ = 0;
  Issue(api, listener, todos_.at(1), 0);
  for (auto i = 1U; i <= max_dup_ && ongoing_ < max_conc_; ++i) {
    Issue(api, listener, todos_.at(0), i);
  }
}
void ipfs::Scheduler::Issue(std::shared_ptr<FrameworkApi> api,
                            std::shared_ptr<DagListener>& listener,
                            std::vector<Todo> todos,
                            unsigned up_to) {
  for (auto& todo : todos) {
    if (todo.dup_count_ <= up_to) {
      auto assign = [&todo](auto& gw) { return gw.accept(todo.suffix); };
      auto it = std::find_if(gateways_.begin(), gateways_.end(), assign);
      if (it != gateways_.end()) {
        todo.dup_count_++;
        api->InitiateGatewayRequest({it->url_prefix(), todo.suffix, this},
                                    listener);
        if (++ongoing_ >= max_conc_) {
          return;
        }
      }
    }
  }
  //  LOG(INFO) << "No pending request has fewer than " << up_to << " requests."
  //  ;
}
std::string ipfs::Scheduler::DetectCompleteFailure() const {
  for (auto& todo : todos_.at(0)) {
    auto tried = [&todo](auto& gw) { return gw.PreviouslyFailed(todo.suffix); };
    if (std::all_of(gateways_.begin(), gateways_.end(), tried)) {
      return todo.suffix;
    }
  }
  return "";
}
void ipfs::Scheduler::CheckSwap(std::size_t index) {
  if (index + 1UL < gateways_.size() &&
      gateways_[index + 1UL] < gateways_[index]) {
    std::swap(gateways_[index], gateways_[index + 1UL]);
  }
}

ipfs::BusyGateway::BusyGateway(std::string pre,
                               std::string suf,
                               Scheduler* sched)
    : prefix_(pre), suffix_(suf), scheduler_{sched}, maybe_offset_(0UL) {}
ipfs::BusyGateway::BusyGateway(BusyGateway&& rhs)
    : prefix_(rhs.prefix_),
      suffix_(rhs.suffix_),
      scheduler_(rhs.scheduler_),
      maybe_offset_(0UL) {
  LOG(INFO) << "BusyGateway<mov ctor>(" << prefix_ << ',' << suffix_ << ')';
  rhs.prefix_.clear();
  rhs.suffix_.clear();
  rhs.scheduler_ = nullptr;
  rhs.maybe_offset_ = 0UL;
}
ipfs::BusyGateway::~BusyGateway() {
  if (*this && get()) {
    (*this)->TaskCancelled();
    if (scheduler_ && scheduler_->ongoing_) {
      scheduler_->ongoing_--;
    }
  }
}
ipfs::Gateway* ipfs::BusyGateway::get() {
  if (!scheduler_ || prefix_.empty() || suffix_.empty()) {
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
ipfs::Gateway* ipfs::BusyGateway::operator->() {
  return get();
}
ipfs::Gateway& ipfs::BusyGateway::operator*() {
  assert(get());  // TODO DCHECK
  return *get();
}
ipfs::BusyGateway::operator bool() const {
  return scheduler_ && prefix_.size() && suffix_.size();
}
void ipfs::BusyGateway::reset() {
  std::clog << static_cast<void*>(this) << "::reset()";
  if (scheduler_) {
    scheduler_->ongoing_--;
    for (auto& todos : scheduler_->todos_) {
      for (auto& todo : todos) {
        if (todo.suffix == suffix_) {
          todo.dup_count_--;
          break;
        }
      }
    }
  }
  scheduler_ = nullptr;
  prefix_.clear();
  suffix_.clear();
}
void ipfs::BusyGateway::Success(Gateways& g,
                                std::shared_ptr<FrameworkApi> api,
                                std::shared_ptr<DagListener>& listener) {
  if (prefix_.empty()) {
    return;
  }
  g.promote(prefix_);
  DCHECK(get());
  get()->TaskSuccess();
  for (auto& todos : scheduler_->todos_) {
    auto it = std::find_if(todos.begin(), todos.end(),
                           [this](auto& t) { return t.suffix == suffix_; });
    if (it != todos.end()) {
      todos.erase(it);
    }
  }
  if (maybe_offset_) {
    scheduler_->CheckSwap(--maybe_offset_);
  }
  scheduler_->IssueRequests(api, listener);
  reset();
}
void ipfs::BusyGateway::Failure(Gateways& g,
                                std::shared_ptr<FrameworkApi> api,
                                std::shared_ptr<DagListener>& listener) {
  DCHECK(prefix_.size() > 0U);
  g.demote(prefix_);
  get()->TaskFailed();
  scheduler_->CheckSwap(maybe_offset_);
  scheduler_->IssueRequests(api, listener);
  reset();
}

std::ostream& operator<<(std::ostream& s, ipfs::Scheduler::Priority p) {
  return s << (p == ipfs::Scheduler::Priority::Required ? "Required"
                                                        : "Optional");
}

ipfs::Scheduler::Todo::Todo(std::string sfx) : suffix(sfx) {}
ipfs::Scheduler::Todo::Todo(Todo const& r)
    : suffix(r.suffix), listeners(r.listeners), dup_count_(r.dup_count_) {}
ipfs::Scheduler::Todo::~Todo() {}
