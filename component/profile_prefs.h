#ifndef IPFS_PROFILE_PREFS_H_
#define IPFS_PROFILE_PREFS_H_

#include "base/component_export.h"

#include <string>

class PrefRegistrySimple;

namespace ipfs {
COMPONENT_EXPORT(IPFS)
void RegisterProfilePrefs(PrefRegistrySimple* registry,
                          std::string const& locale = "");
}  // namespace ipfs

#endif  // IPFS_PROFILE_PREFS_H_
