#ifndef IPFS_CHROMIUM_GATEWAY_CONFIG_H
#define IPFS_CHROMIUM_GATEWAY_CONFIG_H

#include <ipfs_client/gateway_spec.h>

#include <cstdint>
#include <optional>
#include <string_view>

namespace ipfs::ctx {
class GatewayConfig {
 public:
  virtual ~GatewayConfig() noexcept {}
  virtual std::optional<GatewaySpec> GetGateway(std::size_t index) const = 0;
  virtual unsigned GetGatewayRate(std::string_view) = 0;
  virtual void SetGatewayRate(std::string_view, unsigned) = 0;
  virtual void AddGateway(std::string_view) = 0;
};
}  // namespace ipfs::ctx

#endif  // IPFS_CHROMIUM_GATEWAY_CONFIG_H
