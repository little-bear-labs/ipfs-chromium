#include <ipfs_client/networking_api.h>
#include <ipfs_client/scheduler.h>

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
                              std::shared_ptr<DagListener> listener,
                              std::string const& suffix,
                              Priority p) {
  if (suffix.empty()) {
    LOG(ERROR) << "Do not issue a request with no task!";

  } else {
    auto& todo = task2todo_[suffix];
    todo.priority = std::max(todo.priority, p);
  }
  IssueRequests(api, listener);
}
void ipfs::Scheduler::IssueRequests(std::shared_ptr<NetworkingApi> api,
                                    std::shared_ptr<DagListener>& listener) {
  //  LOG(INFO) << "Scheduler::IssueRequests";
  decltype(task2todo_)::value_type* unmet = nullptr;
  auto assign = [this, api, listener](auto& gw, auto& task, auto& todo,
                                      auto need) {
    if (gw.accept(task, need)) {
      auto req = api->InitiateGatewayRequest(
          BusyGateway{gw.url_prefix(), task, this}, listener);
      todo.requests.insert(req);
      LOG(INFO) << "Initiated request " << req->url() << " (" << need << ')';
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
    LOG(WARNING) << "Max load.";
    //    auto more_list = list_gen_();
    //    gateways_.insert(gateways_.end(), more_list.begin(), more_list.end());
  }
  UpdateDevPage();
}

bool ipfs::Scheduler::DetectCompleteFailure(std::string task) const {
  auto fail_count =
      std::count_if(gateways_.begin(), gateways_.end(),
                    [&task](auto& g) { return g.PreviouslyFailed(task); });
  return fail_count < static_cast<long>(gateways_.size());
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

ipfs::Scheduler::Todo::Todo() {}
ipfs::Scheduler::Todo::~Todo() {}
long ipfs::Scheduler::Todo::under_target() const {
  long target = priority;
  return target - static_cast<long>(requests.size());
}