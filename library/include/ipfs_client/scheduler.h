#ifndef IPFS_SCHEDULER_H_
#define IPFS_SCHEDULER_H_

#include "gateways.h"

#include <ctime>
#include <functional>
#include <iosfwd>
#include <memory>
#include <set>

namespace ipfs {
class DagListener;
class FrameworkApi;
class Gateway;
class Scheduler;

class BusyGateway {
 public:
  BusyGateway(BusyGateway const&) = delete;
  BusyGateway(BusyGateway&&);
  ~BusyGateway();
  Gateway& operator*();
  Gateway* operator->();
  Gateway* get();
  explicit operator bool() const;
  void reset();
  bool operator==(BusyGateway const&) const;

  void Success(Gateways&,
               std::shared_ptr<FrameworkApi>,
               std::shared_ptr<DagListener>&);
  void Failure(Gateways&,
               std::shared_ptr<FrameworkApi>,
               std::shared_ptr<DagListener>&);

 private:
  friend class Scheduler;
  BusyGateway(std::string, std::string, Scheduler*);

  std::string prefix_;
  std::string suffix_;
  Scheduler* scheduler_;
  std::size_t maybe_offset_;
};
class Scheduler {
 public:
  explicit Scheduler(GatewayList&& initial_list,
                     unsigned max_concurrent_requests = 20,
                     unsigned duplication_waste_tolerance = 3);
  ~Scheduler();
  enum Result { Scheduled, InProgress, Failed };
  enum class Priority { Required, Optional };
  void Enqueue(std::shared_ptr<FrameworkApi>,
               std::shared_ptr<DagListener> listener,
               std::string const& suffix,
               Priority);
  void IssueRequests(std::shared_ptr<FrameworkApi>,
                     std::shared_ptr<DagListener>& listener);
  std::string DetectCompleteFailure() const;

 private:
  friend class BusyGateway;
  GatewayList gateways_;
  unsigned const max_conc_;
  unsigned const max_dup_;
  unsigned ongoing_ = 0;
  std::time_t fudge_ = 0;
  struct Todo {
    Todo(Todo const&);
    Todo(std::string);
    ~Todo();
    std::string suffix;
    std::set<std::shared_ptr<DagListener>> listeners;
    unsigned dup_count_ = 0;
  };
  std::array<std::vector<Todo>, 2> todos_;

  void Issue(std::shared_ptr<FrameworkApi>,
             std::shared_ptr<DagListener>&,
             std::vector<Todo> todos,
             unsigned up_to);
  void CheckSwap(std::size_t);
};

}  // namespace ipfs

std::ostream& operator<<(std::ostream&, ipfs::Scheduler::Priority);

#endif  // IPFS_SCHEDULER_H_
