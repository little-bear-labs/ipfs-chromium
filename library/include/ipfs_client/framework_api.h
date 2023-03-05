#ifndef IPFS_FRAMEWORK_API_H_
#define IPFS_FRAMEWORK_API_H_

#include "scheduler.h"

#include <memory>

namespace ipfs {

/**
 * @brief Subclass FrameworkApi to provide functionality from whatever
 *    environment you're using this library in.
 */
class FrameworkApi : public std::enable_shared_from_this<FrameworkApi> {
 public:
  virtual void RequestByCid(std::string cid, Scheduler::Priority) = 0;

  virtual void InitiateGatewayRequest(BusyGateway) = 0;
  virtual void ReceiveBlockBytes(std::string_view) = 0;
  virtual void BlocksComplete(std::string mime_type) = 0;

  virtual std::string MimeType(std::string extension,
                               std::string_view content,
                               std::string const& url) const = 0;

  virtual std::string UnescapeUrlComponent(std::string_view) const = 0;
  virtual void FourOhFour(std::string_view, std::string_view) = 0;
};

}  // namespace ipfs

#endif
