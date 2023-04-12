#include <ipfs_client/scheduler.h>

#include <ipfs_client/name_listener.h>
#include <ipfs_client/networking_api.h>

#include "log_macros.h"

#include <algorithm>
#include <fstream>
#include <iostream>

ipfs::Scheduler::Scheduler(std::function<GatewayList()> list_gen)
    : list_gen_(list_gen), gateways_{list_gen()} {
  LOG(INFO) << "Scheduler ctor";
}

ipfs::Scheduler::~Scheduler() {
  LOG(INFO) << "Scheduler dtor";
}

void ipfs::Scheduler::Enqueue(std::shared_ptr<NetworkingApi> api,
                              std::shared_ptr<DagListener> dag_listener,
                              std::shared_ptr<NameListener> name_listener,
                              std::string const& suffix,
                              Priority p) {
  if (suffix.empty()) {
    LOG(ERROR) << "Do not issue a request with no task!";

  } else {
    auto& todo = task2todo_[suffix];
    todo.priority = std::max(todo.priority, p);
    if (dag_listener) {
      todo.dag_listeners.insert(dag_listener);
    }
    if (name_listener) {
      todo.name_listeners.insert(name_listener);
    }
  }
  IssueRequests(api);
}
void ipfs::Scheduler::IssueRequests(std::shared_ptr<NetworkingApi> api) {
  //  LOG(INFO) << "Scheduler::IssueRequests";
  decltype(task2todo_)::value_type* unmet = nullptr;
  auto assign = [this, api](auto& gw, auto& task, auto& todo, auto need) {
    if (gw.accept(task, need)) {
      auto req =
          api->InitiateGatewayRequest(BusyGateway{gw.url_prefix(), task, this});
      todo.requests.insert(req);
      //      LOG(INFO) << "Initiated request " << req->url() << " (" << need <<
      //      ')';
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
  if (unmet) {
    if (!loaded_) {
      LOG(INFO) << "Saturated.";
      loaded_ = true;
    }
  } else {
    loaded_ = false;
  }
  //  UpdateDevPage();
}

bool ipfs::Scheduler::DetectCompleteFailure(std::string task) const {
  auto fail_count =
      std::count_if(gateways_.begin(), gateways_.end(),
                    [&task](auto& g) { return g.PreviouslyFailed(task); });
  LOG(INFO) << task << " has already failed on " << fail_count << " gateways.";
  return fail_count >= static_cast<long>(gateways_.size());
}
void ipfs::Scheduler::CheckSwap(std::size_t index) {
  if (index + 1UL < gateways_.size() &&
      gateways_[index + 1UL] < gateways_[index]) {
    std::swap(gateways_[index], gateways_[index + 1UL]);
  }
}
void ipfs::Scheduler::UpdateDevPage() {
  {
    std::ofstream f{"temp.devpage.html"};
    f << "<html><title>IPFS Gateway Requests</title>"
      << "<body><p>TODOs: " << task2todo_.size() << "</p><table border=1>\n";
    using namespace std::literals;
    for (auto& e : task2todo_) {
      auto& task = e.first;
      auto& todo = e.second;
      f << "  <tr><td>" << task << "</td><td>\n";
      for (auto& req : todo.requests) {
        f << "   <p>" << req->gateway->url_prefix() << "</p>\n";
      }
      f << "  </td></tr>\n";
    }
    f << "</table></body></html>";
  }
  std::rename("temp.devpage.html", "devpage.html");
}
void ipfs::Scheduler::TaskComplete(std::string const& task) {
  auto todo = task2todo_.find(task);
  if (task2todo_.end() == todo) {
    LOG(WARNING) << "An unknown TODO " << task << " finished.";
    return;
  }
  LOG(INFO) << "Task " << task << " completed with "
            << todo->second.name_listeners.size() << " name listeners.";
  // Don't need to call back on dag listeners because storage covered that?
  for (auto& nl : todo->second.name_listeners) {
    LOG(INFO) << "Notifying a name listener that its listener is ready.";
    nl->Complete();
  }
  task2todo_.erase(todo);
}

ipfs::Scheduler::Todo::Todo() {}
ipfs::Scheduler::Todo::~Todo() {}
long ipfs::Scheduler::Todo::under_target() const {
  long target = priority;
  return target - static_cast<long>(requests.size());
}
