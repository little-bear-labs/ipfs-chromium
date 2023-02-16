#include <ipfs_client/scheduler.h>

#include <algorithm>
#include <iostream>

ipfs::Scheduler::Scheduler(GatewayList&& initial_list,
                           RequestCreator requester,
                           unsigned max_conc,
                           unsigned max_dup)
    : gateways_{initial_list},
      requester_(requester),
      max_conc_(max_conc),
      max_dup_(max_dup) {}

ipfs::Scheduler::~Scheduler() {}

void ipfs::Scheduler::Enqueue(std::string const& suffix, Priority p) {
  todos_.at(static_cast<unsigned>(p)).emplace_back(Todo{suffix, 0});
  IssueRequests();
}
void ipfs::Scheduler::IssueRequests() {
  Issue(todos_.at(0), 0);
  if (ongoing_ >= max_conc_) {
    return;
  }
  Issue(todos_.at(1), 0);
  for (auto i = 1U; i <= max_dup_ && ongoing_ < max_conc_; ++i) {
    Issue(todos_.at(0), i);
  }
}
void ipfs::Scheduler::Issue(std::vector<Todo> todos, unsigned up_to) {
  for (auto& todo : todos) {
    if (todo.dup_count_ <= up_to) {
      auto assign = [&todo](auto& gw) { return gw.accept(todo.suffix); };
      auto it = std::find_if(gateways_.begin(), gateways_.end(), assign);
      if (it != gateways_.end()) {
        todo.dup_count_++;
        requester_({it->url_prefix(), todo.suffix, this});
        if (++ongoing_ >= max_conc_) {
          return;
        } else {
          break;
        }
      }
    }
  }
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
    std::clog << "Does this perform moves?\n";
    std::swap(gateways_[index], gateways_[index + 1UL]);
    std::clog << "Did it?\n";
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
  std::clog << "Moving " << prefix_ << " ... " << suffix_ << '\n';
  rhs.prefix_.clear();
  rhs.suffix_.clear();
  rhs.scheduler_ = nullptr;
  rhs.maybe_offset_ = 0UL;
}
ipfs::BusyGateway::~BusyGateway() {
  if (*this && get()) {
    (*this)->TaskCancelled();
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
  std::clog << prefix_ << suffix_ << " BusyGateway::reset()\n";
  scheduler_->ongoing_--;
  for (auto& todos : scheduler_->todos_) {
    for (auto& todo : todos) {
      if (todo.suffix == suffix_) {
        todo.dup_count_--;
        break;
      }
    }
  }
  prefix_.clear();
  suffix_.clear();
}
void ipfs::BusyGateway::Success(Gateways& g) {
  std::clog << "BusyGateway::Success(" << prefix_ << ',' << suffix_ << ")\n";
  assert(prefix_.size() > 0);
  g.promote(prefix_);
  assert(get());
  get()->TaskSuccess();
  if (maybe_offset_) {
    scheduler_->CheckSwap(--maybe_offset_);
  }
  reset();
}
void ipfs::BusyGateway::Failure(Gateways& g) {
  std::clog << "BusyGateway::Failure(" << prefix_ << ',' << suffix_ << ")\n";
  g.demote(prefix_);
  get()->TaskFailed();
  scheduler_->CheckSwap(maybe_offset_);
  reset();
}
