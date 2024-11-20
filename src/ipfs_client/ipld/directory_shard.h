#ifndef IPFS_DIRECTORY_SHARD_H_
#define IPFS_DIRECTORY_SHARD_H_ 1

#include <ipfs_client/ipld/dag_node.h>

namespace ipfs::ipld {
/*! A node in a sharded (HAMT) directory
 */
class DirShard : public DagNode {
  std::uint64_t const fanout_;

  ResolveResult resolve(ResolutionState&) override;
  DirShard* as_hamt() override;

  std::vector<std::string> hexhash(std::string_view path_element) const;
  using HashIter = std::vector<std::string>::const_iterator;
  ResolveResult resolve_internal(HashIter,
                                 HashIter,
                                 std::string_view,
                                 ResolutionState&);
  std::size_t hex_width() const;
  std::string listing_json() const;

 public:
  /*! Construct with a known fanout
   *  @param fanout The number of buckets
   */
  explicit DirShard(std::uint64_t fanout = 256UL);
  ~DirShard() noexcept override;
};
}  // namespace ipfs::ipld

#endif  // IPFS_DIRECTORY_SHARD_H_
