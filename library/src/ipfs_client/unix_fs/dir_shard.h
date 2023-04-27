#ifndef IPFS_DIR_SHARD_H_
#define IPFS_DIR_SHARD_H_

#include "node_helper.h"

namespace ipfs {
class NetworkingApi;
class UnixFsPathResolver;
namespace unix_fs {
class DirShard : public NodeHelper {
 public:
  DirShard(std::string next_path_element);
  ~DirShard() noexcept override;

 private:
  std::string next_path_element_;
  std::vector<std::string> hamt_hexs_;
  int skips_per_pass_ = 0;

  bool Process(std::unique_ptr<NodeHelper>&,
               std::shared_ptr<DagListener>,
               std::function<void(std::string, Priority)>,
               std::string&) override;

  void HashPathElement(std::uint64_t fanout);
  void SomePrefetch(std::function<void(std::string, Priority)>);
};

}  // namespace unix_fs

}  // namespace ipfs

#endif  // IPFS_DIR_SHARD_H_
