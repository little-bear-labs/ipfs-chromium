#include <ipfs_client/ctx/transitory_gateway_config.h>

#include "ipfs_client/ctx/default_gateways.h"
#include "log_macros.h"

#include <algorithm>

using Self = ipfs::ctx::TransitoryGatewayConfig;

Self::TransitoryGatewayConfig() {
  if (!LoadGatewaysFromEnvironmentVariable(*this)) {
    LoadStaticGatewayList(*this);
  }
  std::sort(gateways_.begin(), gateways_.end(),
            [](auto& a, auto& b) { return a.prefix < b.prefix; });
}

std::optional<ipfs::GatewaySpec> Self::GetGateway(std::size_t index) const {
  if (index < gateways_.size()) {
    return gateways_.at(index);
  }
  return std::nullopt;
}
void Self::AddGateway(std::string_view prefix, unsigned at_rate) {
  auto it = FindGateway(prefix);
  if (gateways_.end() != it && it->prefix == prefix) {
    it->rate++;
  } else {
    gateways_.insert(it, GatewaySpec{std::string{prefix}, at_rate});
    DCHECK(
        std::is_sorted(gateways_.begin(), gateways_.end(),
                       [](auto& a, auto& b) { return a.prefix < b.prefix; }));
  }
}
void Self::SetGatewayRate(std::string_view prefix, unsigned int rate) {
  auto it = FindGateway(prefix);
  if (gateways_.end() != it && it->prefix == prefix) {
    VLOG(2) << "Set gateway rate for " << prefix << " to " << rate;
    it->rate = rate;
  } else {
    LOG(INFO) << "Attempted to set the rate of an unknown gateway " << prefix
              << " to " << rate;
  }
}
unsigned Self::GetGatewayRate(std::string_view prefix) {
  auto i = FindGateway(prefix);
  return gateways_.end() == i ? 60U : i->rate;
}
auto Self::FindGateway(std::string_view prefix)
    -> std::vector<GatewaySpec>::iterator {
  auto cmp = [](auto& g, std::string_view p) { return g.prefix < p; };
  return std::lower_bound(gateways_.begin(), gateways_.end(), prefix, cmp);
}
unsigned Self::RoutingApiDiscoveryDefaultRate() const {
  return 60U;
}
bool Self::RoutingApiDiscoveryOfUnencryptedGateways() const {
  return false;
}
int Self::GetTypeAffinity(std::string_view, gw::GatewayRequestType) const {
  return 9;
}
void Self::SetTypeAffinity(std::string_view, gw::GatewayRequestType, int) {}
