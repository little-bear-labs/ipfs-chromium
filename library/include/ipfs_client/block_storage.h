#ifndef IPFS_BLOCKS_H_
#define IPFS_BLOCKS_H_

#include "block.h"
#include "vocab/flat_mapset.h"

#include <string>
#include <string_view>
#include <vector>

namespace ipfs {
class BlockStorage {
 public:
  BlockStorage();
  ~BlockStorage() noexcept;

  bool Store(std::string&& cid, Block&& block);
  Block const* Get(std::string const& cid) const;

  struct Resolution {
    Resolution();
    ~Resolution() noexcept;
    std::string contents;  // NOT NECESSARILY TEXT (just a sequence of bytes)
    std::string mime_;
    std::vector<std::string> required_cids;
    std::vector<std::string> prefetch_cids;
  };

  //"root" CID may be a bit misleading. It's the block the path is relative to.
  Resolution AttemptResolve(std::string const& root_cid,
                            std::string_view unixfs_path) const;

 private:
  flat_map<std::string, Block> cid2node_;

  void AttemptResolveInner(Resolution&,
                           std::string const&,
                           std::string_view) const;
};
}  // namespace ipfs

#endif  // IPFS_BLOCKS_H_
