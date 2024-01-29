#ifndef IPFS_CHROMIUM_IPFS_CONTEXT_H_
#define IPFS_CHROMIUM_IPFS_CONTEXT_H_

#include "dns_txt_request.h"
#include "preferences.h"

#include <ipfs_client/block_storage.h>
#include <ipfs_client/client.h>

#include <base/memory/raw_ref.h>
#include <base/time/time.h>

#include <vocab/raw_ptr.h>

#include <map>

class PrefService;

namespace ipfs {
class InterRequestState;

std::shared_ptr<Client> CreateContext(InterRequestState&, PrefService* prefs);

}  // namespace ipfs

#endif  // IPFS_CHROMIUM_IPFS_CONTEXT_H_
