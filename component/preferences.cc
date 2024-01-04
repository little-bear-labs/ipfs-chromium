#include "preferences.h"

#include <ipfs_client/gateways.h>

#include <base/logging.h>
#include <components/prefs/pref_registry_simple.h>
#include <components/prefs/pref_service.h>

#include <ipfs_client/gateways.h>

namespace {
std::string const kRateLimit{"ipfs.gateways.rate_limits"};
}

void ipfs::RegisterPreferences(PrefRegistrySimple* service) {
  base::Value::Dict vals;
  for (auto& gw : Gateways::DefaultGateways()) {
    vals.Set(gw.prefix, static_cast<int>(gw.rate));
  }
  VLOG(2) << "Registering ipfs.gateways preference with a default value "
             "that contains "
          << vals.size() << " entries.";
  for (auto [k, v] : vals) {
    DCHECK(v.is_int());
  }
  service->RegisterDictionaryPref(kRateLimit, std::move(vals));
}
using Rates = ipfs::GatewayRates;
Rates::GatewayRates(PrefService* prefs) : prefs_{prefs} {
  if (prefs) {
    last_ = prefs->GetDict(kRateLimit).Clone();
    for (auto [k, v] : last_) {
      auto i = v.GetInt();
      curr_.Set(k, std::max(i, 1));
    }
    VLOG(1) << "Initialized with " << curr_.size() << " gateways.";
  } else {
    LOG(ERROR)
        << "Reading preferences without a preferences service is not great.";
  }
}

std::pair<std::string const*, unsigned> Rates::at(std::size_t index) const {
  if (index >= curr_.size()) {
    return {nullptr, 0U};
  }
  auto it = std::next(curr_.cbegin(), index);
  auto* p_k = &(it->first);
  auto v = static_cast<unsigned>(std::max(0, it->second.GetInt()));
  return {p_k, v};
}
unsigned Rates::GetRate(std::string_view k) const {
  auto i = std::max(0, curr_.FindInt(k).value_or(0));
  return static_cast<unsigned>(i);
}
void Rates::SetRate(std::string_view k, unsigned val) {
  auto i = static_cast<int>(std::min(val, static_cast<unsigned>(INT_MAX)));
  auto old = curr_.contains(k);
  curr_.Set(k, i);
  if (!old) {
    LOG(INFO) << "Added new gateway: " << k << '@' << val;
    // TODO - I believe the calls to save here need to be sent to UI thread
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
std::size_t Rates::delta() const {
  std::size_t rv = 0;
  for (auto [k, v] : curr_) {
    auto d = std::abs(v.GetInt() - last_.FindInt(k).value_or(0));
    rv += static_cast<std::size_t>(d);
  }
  return rv;
}
void Rates::save() {
  changes = 0;
  last_ = curr_.Clone();
  update_thresh++;
  prefs_->SetDict(kRateLimit, last_.Clone());
}
