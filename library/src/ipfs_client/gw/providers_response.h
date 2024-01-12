#ifndef IPFS_CHROMIUM_PROVIDERS_RESPONSE_H
#define IPFS_CHROMIUM_PROVIDERS_RESPONSE_H

#include <string_view>

namespace ipfs {
class ContextApi;
}

namespace ipfs::providers {
bool ProcessResponse(std::string_view json_str, ContextApi& api);
}

#endif  // IPFS_CHROMIUM_PROVIDERS_RESPONSE_H
