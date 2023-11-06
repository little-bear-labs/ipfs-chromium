#ifndef IPFS_BLOCK_REQUEST_SPLITTER_H_
#define IPFS_BLOCK_REQUEST_SPLITTER_H_

#include "requestor.h"

namespace ipfs {
class ContextApi;
}

namespace ipfs::gw {
class BlockRequestSplitter final : public Requestor {
  HandleOutcome handle(RequestPtr) override;
  std::string_view name() const override;
};
}  // namespace ipfs::gw

#endif  // IPFS_BLOCK_REQUEST_SPLITTER_H_
