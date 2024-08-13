#ifndef IPFS_EXTRA_PART_H_
#define IPFS_EXTRA_PART_H_

#include <chrome/browser/chrome_browser_main_extra_parts.h>

class IpfsExtraParts : public ChromeBrowserMainExtraParts {
  void PostProfileInit(Profile* profile, bool is_initial_profile) override;
};

#endif  // IPFS_EXTRA_PART_H_
