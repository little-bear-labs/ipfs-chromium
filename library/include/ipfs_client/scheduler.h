#ifndef IPFS_SCHEDULER_H_
#define IPFS_SCHEDULER_H_

#include "block_requestor.h"
#include "busy_gateway.h"
#include "gateways.h"
#include "url_spec.h"

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

/*!
 * \brief Matchmaker for Gateway & Task
 */
class Scheduler {
 public:
  /*!
   * \brief Gateways to use. Sort order relevant.
   */
  explicit Scheduler(std::function<GatewayList()> list_gen);
  ~Scheduler();

  /*!
   * \brief Request a task be eventually handled
   * \param api - General APIs that may be used during processing
   * \param  dl - Who's waiting on some bytes. Typically null for ipns requests.
   * \param  nl - Who's waiting on resolution. Typically null for ipfs requests.
   * \param sfx - The end to the URL, mixed and matched with gateway prefixes.
   * \param pio - How urgent is this request?
   */
  void Enqueue(std::shared_ptr<ContextApi> api,
               std::shared_ptr<DagListener> dl,
               std::shared_ptr<NameListener> nl,
               std::string const& sfx,
               std::string_view accept,
               Priority pio,
               std::shared_ptr<gw::GatewayRequest> top);

  /*!
   * \brief Check enqueued requests to see if a gateway request should be made
   * \param api - General APIs that may be used during processing
   * \return Whether all enqueued requests are pending (false=saturated)
   */
  bool IssueRequests(std::shared_ptr<ContextApi> api);

  /*!
   * \brief Check whether the set of gateways have ALL failed this task
   * \param task - The task one might consider not enqueuing again.
   * \return Whether the candidate gateway list is exhausted
   */
  bool DetectCompleteFailure(UrlSpec const& task) const;

  /*!
   * \brief Indicate this task has completed
   * \param task - URL suffix of the task in question
   */
  void TaskComplete(UrlSpec const& task);

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
    std::set<std::shared_ptr<gw::GatewayRequest>> source_reqs;
    Priority priority;
    long under_target() const;
  };
  std::map<UrlSpec, Todo> task2todo_;

  void Issue(std::shared_ptr<ContextApi>,
             std::shared_ptr<DagListener>&,
             std::vector<Todo> todos,
             unsigned up_to);
  void CheckSwap(std::size_t);
};

}  // namespace ipfs

#endif  // IPFS_SCHEDULER_H_
