#ifndef IPFS_CHROMIUM_GATEWAY_CONFIG_H
#define IPFS_CHROMIUM_GATEWAY_CONFIG_H

#include <ipfs_client/gateway_spec.h>

#include <cstdint>
#include <optional>
#include <string_view>

namespace ipfs::ctx {
/*! Configuration for IPFS gateways - which to use, how, etc.
 */
class GatewayConfig {
 public:
  virtual ~GatewayConfig() noexcept {}
  /*! Position-based access (indexing)
   *  @param index The 0-based index of the gateway in question
   *  @return The spec for the gateway iff index < count of gateways, nullopt otherwise
   */
  virtual std::optional<GatewaySpec> GetGateway(std::size_t index) const = 0;
  virtual unsigned GetGatewayRate(std::string_view url_prefix) = 0;
  virtual int GetTypeAffinity(std::string_view url_prefix,
                              gw::GatewayRequestType) const = 0;
  virtual void SetTypeAffinity(std::string_view url_prefix,
                               gw::GatewayRequestType,
                               int) = 0;

  // These 2 calls are similar, but AddGateway will ignore rpm if the gw is
  // already present,
  //   while SetGatewayRate will override it, as that's its primary purpose.
  virtual void SetGatewayRate(std::string_view url_prefix, unsigned rpm) = 0;
  virtual void AddGateway(std::string_view url_prefix, unsigned rpm) = 0;

  // 0 implies off i.e. don't do this discovery
  virtual unsigned RoutingApiDiscoveryDefaultRate() const = 0;
  virtual bool RoutingApiDiscoveryOfUnencryptedGateways() const = 0;
};
}  // namespace ipfs::ctx

#endif  // IPFS_CHROMIUM_GATEWAY_CONFIG_H
