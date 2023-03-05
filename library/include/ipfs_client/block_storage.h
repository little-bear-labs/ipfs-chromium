#ifndef IPFS_BLOCKS_H_
#define IPFS_BLOCKS_H_

#include "block.h"
#include "vocab/flat_mapset.h"

#include <string>
#include <string_view>
#include <vector>

namespace ipfs {
class FrameworkApi;
class UnixFsPathResolver;

class BlockStorage {
 public:
  BlockStorage();
  BlockStorage(BlockStorage const&) = delete;
  ~BlockStorage() noexcept;

  bool Store(std::shared_ptr<FrameworkApi>,
             std::string const& cid,
             Block&& block);
  Block const* Get(std::string const& cid) const;
  void AddListening(UnixFsPathResolver*);
  void StopListening(UnixFsPathResolver*);

 private:
  flat_map<std::string, Block> cid2node_;
  flat_set<UnixFsPathResolver*> listening_;

  void CheckDoneListening();
};
}  // namespace ipfs

#endif  // IPFS_BLOCKS_H_
