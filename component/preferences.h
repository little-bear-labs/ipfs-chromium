#ifndef IPFS_PREFERENCES_H_INCLUDED
#define IPFS_PREFERENCES_H_INCLUDED 1

#include "export.h"

#include <base/memory/raw_ptr.h>
#include <base/values.h>
#include <ipfs_client/gateways.h>

#include <ranges>

class PrefRegistrySimple;
class PrefService;

namespace ipfs {
COMPONENT_EXPORT(IPFS) void RegisterPreferences(PrefRegistrySimple*);
class GatewayRates {
  raw_ptr<PrefService> prefs_;
  base::Value::Dict last_;
  base::Value::Dict curr_;
  std::size_t changes = 0;
  std::size_t update_thresh = 1;

  void save();
  std::size_t delta() const;

 public:
  GatewayRates(PrefService*);

  unsigned GetRate(std::string_view) const;
  void SetRate(std::string_view, unsigned);
  std::pair<std::string const*, unsigned> at(std::size_t index) const;
};
}

#endif
