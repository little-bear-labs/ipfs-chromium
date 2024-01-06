#ifndef IPFS_BLOCK_REQUEST_SPLITTER_H_
#define IPFS_BLOCK_REQUEST_SPLITTER_H_

#include "requestor.h"

#include <array>
#include <string>

namespace ipfs {
class ContextApi;
}

namespace ipfs::gw {
class BlockRequestSplitter final : public Requestor {
  std::array<std::string, 32> recent_provider_requests;
  std::size_t old_provider_request = 0UL;
  HandleOutcome handle(RequestPtr) override;
  std::string_view name() const override;
};
}  // namespace ipfs::gw

#endif  // IPFS_BLOCK_REQUEST_SPLITTER_H_
