#ifndef IPFS_CHUNK_H_
#define IPFS_CHUNK_H_

#include <ipfs_client/ipld/dag_node.h>

namespace ipfs::ipld {
class Chunk : public DagNode {
  std::string const data_;

  ResolveResult resolve(ResolutionState&) override;

 public:
  explicit Chunk(std::string);
  ~Chunk() noexcept override;
};
}  // namespace ipfs::ipld

#endif  // IPFS_CHUNK_H_
