#ifndef IPFS_SCHEDULER_H_
#define IPFS_SCHEDULER_H_

#include "busy_gateway.h"
#include "gateways.h"
#include "networking_api.h"

#include <ctime>
#include <functional>
#include <iosfwd>
#include <map>
#include <memory>
#include <set>

namespace ipfs {
class DagListener;
class NetworkingApi;
class Gateway;
class Scheduler;

class Scheduler {
 public:
  explicit Scheduler(std::function<GatewayList()> list_gen);
  ~Scheduler();
  enum Result { Scheduled, InProgress, Failed };
  void Enqueue(std::shared_ptr<NetworkingApi>,
               std::shared_ptr<DagListener> listener,
               std::string const& suffix,
               Priority);
  void IssueRequests(std::shared_ptr<NetworkingApi>,
                     std::shared_ptr<DagListener>& listener);
  bool DetectCompleteFailure(std::string task) const;

 private:
  std::function<GatewayList()> list_gen_;
  friend class BusyGateway;
  GatewayList gateways_;
  struct Todo {
    Todo();
    ~Todo();
    std::set<std::shared_ptr<GatewayRequest>> requests;
    Priority priority;
    long under_target() const;
  };
  std::map<std::string, Todo> task2todo_;

  void Issue(std::shared_ptr<NetworkingApi>,
             std::shared_ptr<DagListener>&,
             std::vector<Todo> todos,
             unsigned up_to);
  void CheckSwap(std::size_t);
  void UpdateDevPage();
};

}  // namespace ipfs

#endif  // IPFS_SCHEDULER_H_
