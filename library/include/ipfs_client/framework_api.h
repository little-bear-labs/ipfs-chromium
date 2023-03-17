#ifndef IPFS_FRAMEWORK_API_H_
#define IPFS_FRAMEWORK_API_H_

#include "scheduler.h"

#include <memory>

namespace ipfs {
class DagListener;

/**
 * @brief Subclass FrameworkApi to provide functionality from whatever
 *    environment you're using this library in.
 */
class FrameworkApi : public std::enable_shared_from_this<FrameworkApi> {
 public:
  // Request a single block from gateway(s).
  // The DagListener is mostly about lifetime extension, since it's waiting on
  // something which is waiting on this
  virtual void RequestByCid(std::string cid,
                            std::shared_ptr<DagListener>,
                            Scheduler::Priority) = 0;

  // Send a single http request to its gateway as scheduled
  virtual void InitiateGatewayRequest(BusyGateway,
                                      std::shared_ptr<DagListener>) = 0;

  // Determine a mime type for a given file.
  virtual std::string MimeType(std::string extension,
                               std::string_view content,
                               std::string const& url) const = 0;

  virtual std::string UnescapeUrlComponent(std::string_view) const = 0;
};

}  // namespace ipfs

#endif
