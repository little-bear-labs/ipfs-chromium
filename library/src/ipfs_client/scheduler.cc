#include <ipfs_client/networking_api.h>
#include <ipfs_client/scheduler.h>

#include "log_macros.h"

#include <algorithm>
#include <fstream>
#include <iostream>

ipfs::Scheduler::Scheduler(GatewayList&& initial_list,
                           unsigned max_conc,
                           unsigned max_dup)
    : gateways_{initial_list} {
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
  LOG(INFO) << "Scheduler::IssueRequests";
  auto avail = std::count_if(gateways_.begin(), gateways_.end(),
                             [](auto& g) { return g.current_task().empty(); });
  LOG(INFO) << "We have " << avail << " gateways available for requests.";
  for (auto thresh = 8; avail && thresh >= 0; --thresh) {
    for (auto& e : task2todo_) {
      auto& task = e.first;
      auto& todo = e.second;
      auto need = todo.under_target();
      if (need < 0) {
        continue;
      }
      if (need < thresh) {
        LOG(INFO) << task << " @(" << todo.under_target()
                  << ") doesn't meet threshold of " << thresh;
        continue;
      }
      auto it = std::find_if(gateways_.begin(), gateways_.end(),
                             [&task](auto& gw) { return gw.accept(task); });
      if (gateways_.end() == it) {
        LOG(WARNING) << "Failed to assign gateway for " << task
                     << ". Getting bogged down, or failure?";
      } else {
        auto req = api->InitiateGatewayRequest(
            BusyGateway{it->url_prefix(), task, this}, listener);
        todo.requests.insert(req);
        ++thresh;
        --avail;
        LOG(INFO) << "Initiated a request to " << req->url() << " now have "
                  << avail << " gateways available.";
      }
    }
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
  /*
  {
    std::ofstream f{"temp.devpage.html"};
    f << "<html><title>IPFS Gateway Requests</title><body><table>\n";
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
   */
}

ipfs::Scheduler::Todo::Todo() {}
ipfs::Scheduler::Todo::~Todo() {}
long ipfs::Scheduler::Todo::under_target() const {
  long target = priority;
  return target - static_cast<long>(requests.size());
}