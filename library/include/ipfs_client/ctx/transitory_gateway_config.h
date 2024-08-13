#ifndef IPFS_CHROMIUM_TRANSITORY_GATEWAY_CONFIG_H
#define IPFS_CHROMIUM_TRANSITORY_GATEWAY_CONFIG_H

#include "gateway_config.h"

#include <vector>

namespace ipfs::ctx {
class TransitoryGatewayConfig : public GatewayConfig {
  std::vector<GatewaySpec> gateways_;

  std::vector<GatewaySpec>::iterator FindGateway(std::string_view);

 public:
  std::optional<GatewaySpec> GetGateway(std::size_t index) const override;
  unsigned GetGatewayRate(std::string_view) override;
  void SetGatewayRate(std::string_view, unsigned) override;
  void AddGateway(std::string_view, unsigned) override;
  unsigned RoutingApiDiscoveryDefaultRate() const override;
  bool RoutingApiDiscoveryOfUnencryptedGateways() const override;
  int GetTypeAffinity(std::string_view url_prefix,
                      gw::GatewayRequestType) const override;
  void SetTypeAffinity(std::string_view url_prefix,
                       gw::GatewayRequestType,
                       int) override;

  TransitoryGatewayConfig();
};
}  // namespace ipfs::ctx

#endif  // IPFS_CHROMIUM_TRANSITORY_GATEWAY_CONFIG_H
