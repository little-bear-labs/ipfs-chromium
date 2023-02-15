#include <ipfs_client/scheduler.h>

#include <algorithm>
#include <iostream>

ipfs::Scheduler::Scheduler(GatewayList&& initial_list,
                           RequestCreator requester,
                           unsigned max_conc,
                           unsigned max_dup)
    : unproven_{initial_list},
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
  std::function<void(Gateway*)> end_f = [this](auto p) { End(p); };
  for (auto& todo : todos) {
    if (todo.dup_count_ <= up_to) {
      auto assign = [&todo](auto& gw) { return gw.accept(todo.suffix); };
      for (GatewayList* gws : {&good_, &unproven_}) {
        auto it = std::find_if(gws->begin(), gws->end(), assign);
        if (it != gws->end()) {
          requester_({&*it, end_f});
          if (++ongoing_ >= max_conc_) {
            return;
          } else {
            break;
          }
        }
      }
    }
  }
}
void ipfs::Scheduler::End(Gateway* p) {
  ongoing_--;
  std::clog << "ongoing: " << ongoing_ << ' ';
  for (auto& ts : todos_) {
    for (auto& t : ts) {
      if (t.suffix == p->current_task() && t.dup_count_ > 0) {
        std::clog << "Decreasing dup_count on " << t.suffix << " to "
                  << (t.dup_count_ - 1) << '\n';
        t.dup_count_--;
      }
    }
  }
  //  if (!p->PreviouslyFailed(p->current_task())) {
  //    auto already_good = std::find_if(good_.begin(), good_.end(), [p](auto&
  //    g) {
  //      return p->url_prefix() == p->url_prefix();
  //    });
  // TODO
  //    if (already_good == good_.end()) {
  //      good_.push_back(*p);
  //    }
  // TODO -
  //     std::sort(good_.begin(), good_.end());
  //  } else {
  //    std::sort(unproven_.begin(), unproven_.end());
  //  }
  p->MakeAvailable();
}
std::string ipfs::Scheduler::DetectCompleteFailure() const {
  for (auto& todo : todos_.at(0)) {
    auto tried = [&todo](auto& gw) { return gw.PreviouslyFailed(todo.suffix); };
    if (std::all_of(good_.begin(), good_.end(), tried) &&
        std::all_of(unproven_.begin(), unproven_.end(), tried)) {
      return todo.suffix;
    }
  }
  return "";
}