#include "preferences.h"

#include "components/ipfs/preferences.h"

void electron_spin::RegisterPreferences(PrefRegistrySimple* reg) {
  ipfs::RegisterPreferences(reg);
}
