#include <ipfs_client/ctx/transitory_gateway_config.h>

#include "ipfs_client/ctx/default_gateways.h"
#include "ipfs_client/gateway_spec.h"
#include "ipfs_client/gw/gateway_request_type.h"
#include "log_macros.h"

#include <algorithm>
#include <optional>
#include <cstddef>
#include <string_view>
#include <string>
#include <vector>

using Self = ipfs::ctx::TransitoryGatewayConfig;

namespace {
constexpr auto ONE_HZ = 60U;
}

Self::TransitoryGatewayConfig() {
  if (!LoadGatewaysFromEnvironmentVariable(*this)) {
    LoadStaticGatewayList(*this);
  }
  std::sort(gateways_.begin(), gateways_.end(),
            [](auto& a, auto& b) { return a.prefix < b.prefix; });
}

auto Self::GetGateway(std::size_t index) const -> std::optional<ipfs::GatewaySpec> {
  if (index < gateways_.size()) {
    return gateways_.at(index);
  }
  return std::nullopt;
}
void Self::AddGateway(std::string_view prefix, unsigned at_rate) {
  auto gw_it = FindGateway(prefix);
  if (gateways_.end() != gw_it && gw_it->prefix == prefix) {
    gw_it->rate++;
  } else {
    gateways_.insert(gw_it, GatewaySpec{std::string{prefix}, at_rate});
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
auto Self::GetGatewayRate(std::string_view prefix) -> unsigned {
  auto i = FindGateway(prefix);
  return gateways_.end() == i ? ONE_HZ : i->rate;
}
auto Self::FindGateway(std::string_view prefix)
    -> std::vector<GatewaySpec>::iterator {
  auto cmp = [](auto& g, std::string_view p) { return g.prefix < p; };
  return std::lower_bound(gateways_.begin(), gateways_.end(), prefix, cmp);
}
auto Self::RoutingApiDiscoveryDefaultRate() const -> unsigned {
  return ONE_HZ;
}
auto Self::RoutingApiDiscoveryOfUnencryptedGateways() const -> bool {
  return true;
}
auto Self::GetTypeAffinity(std::string_view, gw::GatewayRequestType) const -> int {
  return 9;
}
void Self::SetTypeAffinity(std::string_view, gw::GatewayRequestType, int) {}
