#ifndef IPFS_CHUNK_H_
#define IPFS_CHUNK_H_

#include <ipfs_client/ipld/dag_node.h>

namespace ipfs::ipld {
class Chunk : public DagNode {
  std::string const data_;

  ResolveResult resolve(SlashDelimited, BlockLookup, std::string&) override;

 public:
  explicit Chunk(std::string);
  virtual ~Chunk() noexcept;
};
}  // namespace ipfs::ipld

#endif  // IPFS_CHUNK_H_
