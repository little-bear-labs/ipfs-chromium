#ifndef IPFS_CHROMIUM_MOCK_GW_CFG_H
#define IPFS_CHROMIUM_MOCK_GW_CFG_H

#include <ipfs_client/ctx/gateway_config.h>

namespace i = ipfs;
namespace ig = i::gw;

namespace {
struct MockGwCfg final : public ipfs::ctx::GatewayConfig {
  std::vector<i::GatewaySpec> index_gettable_gateways;
  std::optional<i::GatewaySpec> GetGateway(std::size_t i) const {
    if (i < index_gettable_gateways.size()) {
      return index_gettable_gateways.at(i);
    }
    return std::nullopt;
  }
  unsigned GetGatewayRate(std::string_view) { return 120U; }
  std::vector<std::string> gateways_added;
  void AddGateway(std::string_view g) { gateways_added.emplace_back(g); }
  void AddGateway(std::string_view g, unsigned) {
    gateways_added.emplace_back(g);
  }
  void SetGatewayRate(std::string_view, unsigned int) override {}
  unsigned RoutingApiDiscoveryDefaultRate() const { return 9; }
  virtual ~MockGwCfg() noexcept override {}
  bool http_disc_ = true;
  bool RoutingApiDiscoveryOfUnencryptedGateways() const { return http_disc_; }
  int GetTypeAffinity(std::string_view url_prefix,
                      ig::GatewayRequestType) const {
    return 0;
  }
  void SetTypeAffinity(std::string_view url_prefix,
                       ig::GatewayRequestType,
                       int) {}
};
}  // namespace

#endif  // IPFS_CHROMIUM_MOCK_GW_CFG_H
