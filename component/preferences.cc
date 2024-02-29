#include "preferences.h"

#include <ipfs_client/ctx/default_gateways.h>
#include <ipfs_client/ctx/transitory_gateway_config.h>

#include <base/logging.h>
#include <components/prefs/pref_registry_simple.h>
#include <components/prefs/pref_service.h>

namespace {
std::string const kRateLimits{"ipfs.gateways.rate_limits"};
std::string const kDiscoveryRate{"ipfs.gateways.discovery.rate"};
std::string const kDiscoveryOfUnencrypted{"ipfs.gateways.discovery.http"};
}

void ipfs::RegisterPreferences(PrefRegistrySimple* service) {
  ctx::TransitoryGatewayConfig cfg;
  ctx::LoadStaticGatewayList(cfg);
  base::Value::Dict vals;
  for (auto i = 0UL; i < 999UL; ++i) {
    auto gw = cfg.GetGateway(i);
    if (!gw) {
      break;
    }
    vals.Set(gw->prefix, static_cast<int>(gw->rate));
  }
  VLOG(2) << "Registering ipfs.gateways preference with a default value "
             "that contains "
          << vals.size() << " entries.";
  for (auto [k, v] : vals) {
    DCHECK(v.is_int());
  }
  service->RegisterDictionaryPref(kRateLimits, std::move(vals));
  service->RegisterIntegerPref(kDiscoveryRate, 120);
  service->RegisterIntegerPref(kDiscoveryOfUnencrypted, true);
}
using Self = ipfs::ChromiumIpfsGatewayConfig;
Self::ChromiumIpfsGatewayConfig(PrefService* prefs) : prefs_{prefs} {
  if (prefs) {
    last_ = prefs->GetDict(kRateLimits).Clone();
    for (auto [k, v] : last_) {
      auto i = v.GetInt();
      curr_.Set(k, std::max(i, 1));
    }
    VLOG(2) << "Initialized with " << curr_.size() << " gateways.";
  } else {
    LOG(ERROR)
        << "Reading preferences without a preferences service is not great.";
  }
}

std::pair<std::string const*, unsigned> Self::at(std::size_t index) const {
  if (index >= curr_.size()) {
    return {nullptr, 0U};
  }
  auto it = std::next(curr_.cbegin(), index);
  auto* p_k = &(it->first);
  auto v = static_cast<unsigned>(std::max(0, it->second.GetInt()));
  return {p_k, v};
}
unsigned Self::GetGatewayRate(std::string_view k) {
  auto i = std::max(0, curr_.FindInt(k).value_or(0));
  return static_cast<unsigned>(i);
}
void Self::SetGatewayRate(std::string_view k, unsigned val) {
  auto i = static_cast<int>(std::min(val, static_cast<unsigned>(INT_MAX)));
  auto old = curr_.contains(k);
  curr_.Set(k, i);
  if (!old) {
    LOG(INFO) << "Added new gateway: " << k << '@' << val;
    save();
  } else if (++changes > update_thresh) {
    VLOG(2) << "Changing rate for gateway " << k << " to " << val;
    auto d = delta();
    if (d > update_thresh) {
      save();
    } else {
      changes = d / 2;
    }
  }
}
void Self::AddGateway(std::string_view k, unsigned r) {
  curr_.Set(k, curr_.FindInt(k).value_or(r) + 1);
}
auto Self::GetGateway(std::size_t index) const -> std::optional<GatewaySpec> {
  auto [k, r] = at(index);
  if (k) {
    return GatewaySpec{*k, r};
  }
  return std::nullopt;
}
std::size_t Self::delta() const {
  std::size_t rv = 0;
  for (auto [k, v] : curr_) {
    auto d = std::abs(v.GetInt() - last_.FindInt(k).value_or(0));
    rv += static_cast<std::size_t>(d);
  }
  return rv;
}
void Self::save() {
  // Should be called on UI thread
  changes = 0;
  last_ = curr_.Clone();
  update_thresh++;
  prefs_->SetDict(kRateLimits, last_.Clone());
}
unsigned Self::RoutingApiDiscoveryDefaultRate() const {
  return static_cast<unsigned>(prefs_->GetInteger(kDiscoveryRate));
}
bool Self ::RoutingApiDiscoveryOfUnencryptedGateways() const {
  return prefs_->GetBoolean(kDiscoveryOfUnencrypted);
}
