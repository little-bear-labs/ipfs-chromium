#ifndef IPFS_UNIXFS_FILE_H_
#define IPFS_UNIXFS_FILE_H_

#include <ipfs_client/ipld/dag_node.h>

namespace ipfs::ipld {
class UnixfsFile : public DagNode {
  ResolveResult resolve(SlashDelimited, BlockLookup, std::string&) override;

 public:
  virtual ~UnixfsFile() noexcept;
};
}  // namespace ipfs::ipld

#endif  // IPFS_UNIXFS_FILE_H_
