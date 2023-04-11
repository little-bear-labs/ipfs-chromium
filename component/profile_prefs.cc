#include "profile_prefs.h"

#include "base/values.h"
#include "components/prefs/pref_registry_simple.h"

#include <ipfs_client/gateways.h>

#include <iostream>  //TODO rm

void ipfs::RegisterProfilePrefs(PrefRegistrySimple* registry,
                                std::string const&  // locale
) {
  DCHECK(registry);
  if (!registry) {
    return;
  }
  auto dflt = Gateways::DefaultGateways();
  base::Value::Dict pref_val;
  for (auto [k, v] : dflt) {
    std::clog << k << '=' << v << '\n';
    pref_val.Set(k, v);
  }
  std::clog << "\n\n\t registering \"net.ipfs.gateways\"\n\n";
  registry->RegisterDictionaryPref("net.ipfs.gateways", std::move(pref_val));
}
