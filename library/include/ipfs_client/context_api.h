#ifndef IPFS_CONTEXT_API_H_
#define IPFS_CONTEXT_API_H_

#include "busy_gateway.h"

#include <functional>
#include <memory>

namespace ipfs {

class GatewayRequest {
 public:
  BusyGateway gateway;

  GatewayRequest(BusyGateway&&);
  virtual ~GatewayRequest() noexcept;
  std::string task() const;
  std::string url() const;
};

/**
 * @brief Subclass to provide functionality from whatever
 *    environment you're using this library in.
 */
class ContextApi : public std::enable_shared_from_this<ContextApi> {
 public:
  // Send a single http request to its gateway as scheduled
  virtual std::shared_ptr<GatewayRequest> InitiateGatewayRequest(
      BusyGateway) = 0;

  // Determine a mime type for a given file.
  virtual std::string MimeType(std::string extension,
                               std::string_view content,
                               std::string const& url) const = 0;

  virtual std::string UnescapeUrlComponent(std::string_view) const = 0;
  virtual void Discover(std::function<void(std::vector<std::string>)>) = 0;
};

}  // namespace ipfs

#endif
