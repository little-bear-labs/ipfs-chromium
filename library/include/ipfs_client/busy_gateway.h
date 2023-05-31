#ifndef IPFS_BUSY_GATEWAY_H_
#define IPFS_BUSY_GATEWAY_H_

#include <vocab/raw_ptr.h>

#include <memory>
#include <string>

namespace {
struct TestParams;
}

namespace ipfs {
class DagListener;
class Gateway;
class Gateways;
class ContextApi;
class Scheduler;

/*!
 * \brief RAII class embodying the assignment of a given task to a given gateway
 */
class BusyGateway {
 public:
  BusyGateway(BusyGateway const&) = delete;
  BusyGateway(BusyGateway&&);
  ~BusyGateway();

  /*!
   * \name Gateway Access
   *    Pointer semantics to access the underlying gateway.
   */
  ///@{
  Gateway& operator*();
  Gateway* operator->();
  Gateway const* operator->() const;
  Gateway* get();
  explicit operator bool() const;
  void reset();
  ///@}

  //  bool operator==(BusyGateway const&) const;

  /*!
   * \brief Indicate that the task has successfully completed
   * \param gws - a list that may be modified as a result
   * \param api - usable access to the context
   */
  void Success(Gateways& gws, std::shared_ptr<ContextApi> api);

  /*!
   * \brief Indicate that the task has unsuccessfully completed
   * \param gws - a list that may be modified as a result
   * \param api - usable access to the context
   */
  void Failure(Gateways&, std::shared_ptr<ContextApi>);

  /*!
   * \brief What task does this refer to?
   * \return Suffix to the URL being fetched
   */
  std::string const& task() const { return suffix_; }

  /*!
   * \brief The full url intended to be fetched here
   * \return get()->url_prefix() + task()
   */
  std::string url() const { return prefix_ + suffix_; }

  static void TestAccess(TestParams*);

 private:
  friend class Scheduler;
  BusyGateway(std::string, std::string, Scheduler*);

  std::string prefix_;
  std::string suffix_;
  raw_ptr<Scheduler> scheduler_;
  std::size_t maybe_offset_;
};

}  // namespace ipfs

#endif
