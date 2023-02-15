#ifndef IPFS_BLOCKS_H_
#define IPFS_BLOCKS_H_

#include "block.h"
#include "vocab/flat_mapset.h"

#include <string>
#include <string_view>
#include <vector>

namespace ipfs {
class UnixFsPathResolver;

class BlockStorage {
 public:
  BlockStorage();
  ~BlockStorage() noexcept;

  bool Store(std::string const& cid, Block&& block);
  Block const* Get(std::string const& cid) const;
  void AddListening(std::shared_ptr<UnixFsPathResolver>);

 private:
  flat_map<std::string, Block> cid2node_;
  flat_set<std::shared_ptr<UnixFsPathResolver>> listening_;

  void CheckDoneListening();
};
}  // namespace ipfs

#endif  // IPFS_BLOCKS_H_
