#include <ipfs_client/scheduler.h>

#include <ipfs_client/context_api.h>
#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/name_listener.h>
#include <ipfs_client/orchestrator.h>
#include <ipfs_client/response.h>

#include "log_macros.h"

#include <algorithm>
#include <fstream>
#include <iostream>

ipfs::Scheduler::Scheduler(std::function<GatewayList()> list_gen)
    : list_gen_(list_gen), gateways_{list_gen()} {
  VLOG(1) << "Scheduler ctor";
}

ipfs::Scheduler::~Scheduler() {
  LOG(INFO) << "Scheduler dtor";
}

void ipfs::Scheduler::Enqueue(std::shared_ptr<ContextApi> api,
                              std::shared_ptr<DagListener> dag_listener,
                              std::shared_ptr<NameListener> name_listener,
                              std::string const& suffix,
                              std::string_view accept,
                              Priority p,
                              std::shared_ptr<gw::GatewayRequest> top) {
  LOG(INFO) << "Scheduler::Enqueue(...," << suffix << ',' << accept << ',' << p
            << ')';
  if (!top) {
    LOG(ERROR) << "No IpfsRequest?";
  }
  if (suffix.empty()) {
    LOG(ERROR) << "Do not issue a request with no task!";
  } else {
    auto key = UrlSpec{suffix, accept};
    auto& todo = task2todo_[key];
    todo.priority = std::max(todo.priority, p);
    if (dag_listener) {
      todo.dag_listeners.insert(dag_listener);
    }
    if (name_listener) {
      todo.name_listeners.insert(name_listener);
    }
    if (top) {
      todo.source_reqs.insert(top);
    } else {
      LOG(WARNING) << "Enqueue with no top: " << suffix;
    }
  }
  IssueRequests(api);
}
bool ipfs::Scheduler::IssueRequests(std::shared_ptr<ContextApi> api) {
  //  LOG(INFO) << "Scheduler::IssueRequests";
  decltype(task2todo_)::value_type* unmet = nullptr;
  auto assign = [this, api](auto& gw, auto& task, auto& todo, auto need) {
    if (gw.accept(task, need)) {
      BusyGateway bgw{gw.url_prefix(), task, this};
      std::shared_ptr<gw::GatewayRequest> top;
      if (todo.source_reqs.empty()) {
        LOG(ERROR) << "IssueRequests with no top-level requests awaiting!";
      } else {
        top = *todo.source_reqs.begin();  // This is wrong, but so is
                                          // everything about this
      }
      bgw.srcreq = top;
      auto req = api->InitiateGatewayRequest(std::move(bgw));
      todo.requests.insert(req);
      VLOG(2) << "Initiated request " << req->url() << " (" << need << ')'
              << todo.source_reqs.size() << " await.";
      return true;
    }
    return false;
  };
  for (auto& e : task2todo_) {
    auto& task = e.first;
    auto& todo = e.second;
    auto need = todo.under_target();
    for (auto& gw : gateways_) {
      if (assign(gw, task, todo, need)) {
        need = todo.under_target();
      } else if (!unmet || unmet->second.under_target() < need) {
        unmet = &e;
      }
    }
  }
  if (unmet) {
    for (auto& gw : gateways_) {
      if (gw.load() < 2 && assign(gw, unmet->first, unmet->second, 987)) {
        unmet = nullptr;
        break;
      }
    }
  }
  return !unmet;
}

bool ipfs::Scheduler::DetectCompleteFailure(UrlSpec const& task) const {
  auto fail_count =
      std::count_if(gateways_.begin(), gateways_.end(),
                    [&task](auto& g) { return g.PreviouslyFailed(task); });
  // LOG(INFO) << task << " has already failed on " << fail_count
  // << "gateways.";
  return fail_count >= static_cast<long>(gateways_.size());
}
void ipfs::Scheduler::CheckSwap(std::size_t index) {
  if (index + 1UL < gateways_.size() &&
      gateways_[index + 1UL] < gateways_[index]) {
    std::swap(gateways_[index], gateways_[index + 1UL]);
  }
}
void ipfs::Scheduler::TaskComplete(UrlSpec const& task) {
  auto todo = task2todo_.find(task);
  if (task2todo_.end() == todo) {
    VLOG(2) << "An unknown TODO " << task.suffix << " finished.";
    return;
  }
  LOG(INFO) << "Task " << task.suffix << " completed with "
            << todo->second.name_listeners.size() << " name listeners and "
            << todo->second.source_reqs.size() << " IpfsRequest s";
  // Don't need to call back on dag listeners because storage covered that
  for (auto& nl : todo->second.name_listeners) {
    LOG(INFO) << "Notifying a name listener that its listener is ready.";
    nl->Complete();
  }
  for (auto& r : todo->second.source_reqs) {
    r->orchestrator->build_response(r->dependent);
  }
  task2todo_.erase(todo);
}

ipfs::Scheduler::Todo::Todo() {}
ipfs::Scheduler::Todo::~Todo() {}
long ipfs::Scheduler::Todo::under_target() const {
  long target = priority;
  return target - static_cast<long>(requests.size());
}
