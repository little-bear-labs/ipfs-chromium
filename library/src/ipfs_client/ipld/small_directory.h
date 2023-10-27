#ifndef IPFS_UNIXFS_DIRECTORY_H_
#define IPFS_UNIXFS_DIRECTORY_H_

#include "ipfs_client/ipld/link.h"

#include <ipfs_client/ipld/dag_node.h>

namespace ipfs::ipld {
class SmallDirectory : public DagNode {
  ResolveResult resolve(SlashDelimited, BlockLookup, std::string&) override;

 public:
  virtual ~SmallDirectory() noexcept;
};
}  // namespace ipfs::ipld

#endif  // IPFS_UNIXFS_DIRECTORY_H_
