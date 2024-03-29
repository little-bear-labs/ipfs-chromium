#ifndef IPFS_CHROMIUM_MOCK_GW_CFG_H
#define IPFS_CHROMIUM_MOCK_GW_CFG_H

#include <ipfs_client/ctx/gateway_config.h>

namespace i = ipfs;

namespace {
struct MockGwCfg : public ipfs::ctx::GatewayConfig {
  std::optional<i::GatewaySpec> GetGateway(std::size_t) const {
    return std::nullopt;
  }
  unsigned GetGatewayRate(std::string_view) { return 120U; }
  std::vector<std::string> gateways_added;
  void AddGateway(std::string_view g) { gateways_added.emplace_back(g); }
  void SetGatewayRate(std::string_view, unsigned int) override {}
  virtual ~MockGwCfg() noexcept override {}
};
}  // namespace

#endif  // IPFS_CHROMIUM_MOCK_GW_CFG_H
