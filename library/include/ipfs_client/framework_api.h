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
  // Request a single block from gateway(s). TODO - does this really belong
  // here?
  virtual void RequestByCid(std::string cid, Scheduler::Priority) = 0;

  // Send a single http request to its gateway as scheduled
  virtual void InitiateGatewayRequest(BusyGateway) = 0;

  // Sorry for the terrible name.
  //  You're receiving the next N bytes of a requested file, corresponding to a
  //  single block of its DAG
  virtual void ReceiveBlockBytes(std::string_view) = 0;
  // Again, sorry for this name.
  // The file you were recevining in ReceiveBlockBytes is done. You got all the
  // bytes.
  virtual void BlocksComplete(std::string mime_type) = 0;

  // Determine a mime type for a given file.
  virtual std::string MimeType(std::string extension,
                               std::string_view content,
                               std::string const& url) const = 0;

  virtual std::string UnescapeUrlComponent(std::string_view) const = 0;

  // The requested path DOES NOT EXIST, conclusively, regardless of gateway.
  virtual void FourOhFour(std::string_view, std::string_view) = 0;
};

}  // namespace ipfs

#endif
