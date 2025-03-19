#include "ipfs_extra_parts.h"

#include "profiles/profile.h"

#include <components/ipfs/inter_request_state.h>

void IpfsExtraParts::PostProfileInit(Profile* profile, bool /* is_initial_profile */ ) {
  DCHECK(profile);
  ipfs::InterRequestState::CreateForBrowserContext(profile, profile->GetPrefs());
}
