#ifndef IPFS_SCHEDULER_H_
#define IPFS_SCHEDULER_H_

#include "gateways.h"

#include <functional>
#include <memory>

namespace ipfs {
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

  void Success(Gateways&);
  void Failure(Gateways&);

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
  using RequestCreator = std::function<void(BusyGateway)>;
  explicit Scheduler(GatewayList&& initial_list,
                     RequestCreator,
                     unsigned max_concurrent_requests = 10,
                     unsigned duplication_waste_tolerance = 3);
  ~Scheduler();
  enum Result { Scheduled, InProgress, Failed };
  enum class Priority { Required, Optional };
  void Enqueue(std::string const& suffix, Priority);
  void IssueRequests();
  std::string DetectCompleteFailure() const;

 private:
  friend class BusyGateway;
  GatewayList gateways_;
  RequestCreator requester_;
  unsigned const max_conc_;
  unsigned const max_dup_;
  unsigned ongoing_ = 0;
  struct Todo {
    std::string suffix;
    unsigned dup_count_ = 0;
  };
  std::array<std::vector<Todo>, 2> todos_;

  void Issue(std::vector<Todo> todos, unsigned up_to);
  void CheckSwap(std::size_t);
};

}  // namespace ipfs

#endif  // IPFS_SCHEDULER_H_
