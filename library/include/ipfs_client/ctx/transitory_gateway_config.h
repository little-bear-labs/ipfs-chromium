#ifndef IPFS_CHROMIUM_TRANSITORY_GATEWAY_CONFIG_H
#define IPFS_CHROMIUM_TRANSITORY_GATEWAY_CONFIG_H

#include "gateway_config.h"

#include <vector>

namespace ipfs::ctx {
/*! Takes the place of a configuration, but it's not persisted
 */
class TransitoryGatewayConfig : public GatewayConfig {
  std::vector<GatewaySpec> gateways_;

  std::vector<GatewaySpec>::iterator FindGateway(std::string_view);

 public:
  /* @param index 0-based index into list
   * @return The spec at index, or nullopt if index is out-of-bounds
   */
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

  /*! Loads gateways from IPFS_GATEWAY environment or a static list
   */
  TransitoryGatewayConfig();
};
}  // namespace ipfs::ctx

#endif  // IPFS_CHROMIUM_TRANSITORY_GATEWAY_CONFIG_H
