#include "preferences.h"

#include <ipfs_client/ctx/default_gateways.h>
#include <ipfs_client/ctx/transitory_gateway_config.h>

#include <base/logging.h>
#include <components/prefs/pref_registry_simple.h>
#include <components/prefs/pref_service.h>

namespace {
std::string const kGateway{"ipfs.gateway"};
// std::string const kRateLimits{"ipfs.gateways.rate_limits"};
std::string const kDiscoveryRate{"ipfs.discovery.rate"};
std::string const kDiscoveryOfUnencrypted{"ipfs.discovery.http"};

std::string const kRateKey{"max_requests_per_minute"};

base::Value::Dict AsJson(ipfs::GatewaySpec const&);
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
    DCHECK(!(gw->prefix.empty()));
    //    vals.Set(gw->prefix, static_cast<int>(gw->rate));
    vals.Set(gw->prefix, AsJson(*gw));
  }
  service->RegisterDictionaryPref(kGateway, std::move(vals));
  service->RegisterIntegerPref(kDiscoveryRate, 120);
  service->RegisterBooleanPref(kDiscoveryOfUnencrypted, true);
}
using Self = ipfs::ChromiumIpfsGatewayConfig;
Self::ChromiumIpfsGatewayConfig(PrefService* prefs) : prefs_{prefs} {
  if (prefs) {
    last_ = prefs->GetDict(kGateway).Clone();
    curr_ = last_.Clone();
    for (auto [k, v] : curr_) {
      DCHECK(!k.empty());
    }
  } else {
    LOG(ERROR)
        << "Reading preferences without a preferences service is not great.";
  }
}
unsigned Self::GetGatewayRate(std::string_view k) {
  if (auto* d = curr_.FindDict(k)) {
    auto i = d->FindInt(kRateKey).value_or(0);
    return static_cast<unsigned>(i);
  }
  return 0U;
}
void Self::SetGatewayRate(std::string_view k, unsigned val) {
  DCHECK(!k.empty());
  auto* d = curr_.FindDict(k);
  if (!d) {
    LOG(ERROR) << "Don't set a rate for a not-yet-added gateway.";
    AddGateway(k, val);
    return;
  }
  auto i = static_cast<int>(std::min(val, static_cast<unsigned>(INT_MAX)));
  d->Set(kRateKey, i);
  VLOG(1) << "Changing rate for gateway " << k << " to " << val;
  if (++changes > update_thresh) {
    auto delt = delta();
    if (delt > update_thresh) {
      save();
    } else {
      changes = delt / 2;
      LOG(INFO) << "Rate changes total (delta) " << delt;
    }
  }
}
void Self::AddGateway(std::string_view k, unsigned r) {
  DCHECK(!k.empty());
  if (auto* d = curr_.FindDict(k)) {
    d->Set(kRateKey, d->FindInt(kRateKey).value_or(r) + 1);
  } else {
    auto j = AsJson(GatewaySpec{k, r});
    LOG(INFO) << static_cast<void*>(this) << " Adding " << k << " @ " << r
              << " = " << j;
    curr_.Set(k, std::move(j));
  }
}
auto Self::GetGateway(std::size_t index) const -> std::optional<GatewaySpec> {
  if (index >= curr_.size()) {
    return std::nullopt;
  }
  auto it = std::next(curr_.cbegin(), index);
  auto* p_k = &(it->first);
  DCHECK(!(p_k->empty()));
  auto* vals = it->second.GetIfDict();
  if (!vals) {
    LOG(WARNING) << "Gateway values were not Dict!!";
    return std::nullopt;
  }
  auto i = std::max(0, vals->FindInt(kRateKey).value_or(0));
  GatewaySpec result{*p_k, static_cast<unsigned>(i)};
  DCHECK_EQ(result.prefix, *p_k);
  for (auto [k, v] : *vals) {
    if (auto t = gw::from_name(k)) {
      auto idx = static_cast<int>(*t);
      if (idx < static_cast<int>(result.request_type_affinity.size())) {
        result.request_type_affinity[idx] = v.GetInt();
      }
    }
  }
  return result;
}
std::size_t Self::delta() const {
  std::size_t rv = 0;
  for (auto [k, v] : curr_) {
    int c = 0;
    int p = 0;
    if (auto* curr_dict = v.GetIfDict()) {
      c = curr_dict->FindInt(kRateKey).value_or(0);
    }
    if (auto* prev_dict = last_.FindDict(k)) {
      p = prev_dict->FindInt(kRateKey).value_or(0);
    }
    auto off = std::abs(c - p);
    if (off) {
      VLOG(1) << k << " : " << p << " to " << c << " : absdiff=" << off;
      rv += static_cast<std::size_t>(off);
    }
  }
  return rv;
}
void Self::save() {
  VLOG(1) << "Saving preferences. Threshold: " << update_thresh;
  // Should be called on UI thread
  changes = 0;
  last_ = curr_.Clone();
  update_thresh++;
  prefs_->SetDict(kGateway, last_.Clone());
}
unsigned Self::RoutingApiDiscoveryDefaultRate() const {
  auto i = prefs_->GetInteger(kDiscoveryRate);
  VLOG(2) << "RoutingApiDiscoveryDefaultRate=" << i;
  return static_cast<unsigned>(i);
}
bool Self::RoutingApiDiscoveryOfUnencryptedGateways() const {
  return prefs_->GetBoolean(kDiscoveryOfUnencrypted);
}
int Self::GetTypeAffinity(std::string_view url_prefix,
                          gw::GatewayRequestType typ) const {
  if (auto* d = curr_.FindDict(url_prefix)) {
    return d->FindInt(name(typ)).value_or(0);
  }
  return 0;
}
void Self::SetTypeAffinity(std::string_view url_prefix,
                           gw::GatewayRequestType typ,
                           int val) {
  auto nm = name(typ);
  VLOG(1) << "SetTypeAff(" << url_prefix << ',' << nm << ',' << val << ')';
  if (auto* d = curr_.FindDict(url_prefix)) {
    d->Set(nm, val);
  }
}

namespace {
base::Value::Dict AsJson(ipfs::GatewaySpec const& s) {
  base::Value::Dict d;
  auto r = std::max(0U, s.rate);
  d.Set(kRateKey, static_cast<int>(r));
  for (auto i = 0U; i < s.request_type_affinity.size(); ++i) {
    auto n = name(static_cast<ipfs::gw::GatewayRequestType>(i));
    d.Set(n, s.request_type_affinity[i]);
  }
  return d;
}
}  // namespace
