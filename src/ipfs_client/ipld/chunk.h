#ifndef IPFS_CHUNK_H_
#define IPFS_CHUNK_H_

#include <ipfs_client/ipld/dag_node.h>

namespace ipfs::ipld {
/*! A chunk of bytes.
 *  @details The node may have been a raw codec,
 *  or a UnixFS file or UnixFS raw,
 *  but when traversing to get a final response they all kind of behave the same.
 *  It's a leaf.
 */
class Chunk : public DagNode {
  std::string const data_;

  ResolveResult resolve(ResolutionState&) override;

 public:
  /*! Construct
   *  @param bytes The blob itself
   */
  explicit Chunk(std::string bytes);
  ~Chunk() noexcept override;
};
}  // namespace ipfs::ipld

#endif  // IPFS_CHUNK_H_
