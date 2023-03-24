#ifndef IPFS_NETWORKING_API_H_
#define IPFS_NETWORKING_API_H_

#include "busy_gateway.h"

#include <functional>
#include <memory>

namespace ipfs {

using Priority = std::uint_least16_t;

class DagListener;

class GatewayRequest {
 public:
  BusyGateway gateway;
  //  std::shared_ptr<DagListener> listener;

  GatewayRequest(BusyGateway&&);
  virtual ~GatewayRequest() noexcept;
  std::string task() const;
  std::string url() const;
};

/**
 * @brief Subclass to provide functionality from whatever
 *    environment you're using this library in.
 */
class NetworkingApi : public std::enable_shared_from_this<NetworkingApi> {
 public:
  // Request a single block from gateway(s).
  // The DagListener is mostly about lifetime extension, since it's waiting on
  // something which is waiting on this
  virtual void RequestByCid(std::string cid,
                            std::shared_ptr<DagListener>,
                            Priority priority) = 0;

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
