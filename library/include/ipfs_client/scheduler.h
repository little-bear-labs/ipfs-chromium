#ifndef IPFS_SCHEDULER_H_
#define IPFS_SCHEDULER_H_

#include "block_requestor.h"
#include "busy_gateway.h"
#include "gateways.h"

#include <ctime>
#include <functional>
#include <iosfwd>
#include <map>
#include <memory>
#include <set>

namespace ipfs {
class DagListener;
class NameListener;
class ContextApi;
class Gateway;
class GatewayRequest;
class Scheduler;

class Scheduler {
 public:
  explicit Scheduler(std::function<GatewayList()> list_gen);
  ~Scheduler();
  enum Result { Scheduled, InProgress, Failed };
  void Enqueue(std::shared_ptr<ContextApi>,
               std::shared_ptr<DagListener>,
               std::shared_ptr<NameListener>,
               std::string const& suffix,
               Priority);
  bool IssueRequests(std::shared_ptr<ContextApi>);
  bool DetectCompleteFailure(std::string task) const;
  void TaskComplete(std::string const&);

 private:
  std::function<GatewayList()> list_gen_;
  friend class BusyGateway;
  GatewayList gateways_;
  struct Todo {
    Todo();
    ~Todo();
    std::set<std::shared_ptr<GatewayRequest>> requests;
    std::set<std::shared_ptr<DagListener>> dag_listeners;
    std::set<std::shared_ptr<NameListener>> name_listeners;
    Priority priority;
    long under_target() const;
  };
  std::map<std::string, Todo> task2todo_;
  bool saturated_ = false;

  void Issue(std::shared_ptr<ContextApi>,
             std::shared_ptr<DagListener>&,
             std::vector<Todo> todos,
             unsigned up_to);
  void CheckSwap(std::size_t);
  void UpdateDevPage();
};

}  // namespace ipfs

#endif  // IPFS_SCHEDULER_H_
