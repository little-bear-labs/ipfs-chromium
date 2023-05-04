#ifndef IPFS_BLOCKS_H_
#define IPFS_BLOCKS_H_

#include "dag_block.h"
#include "vocab/flat_mapset.h"

#include <string>
#include <string_view>
#include <vector>

namespace ipfs {
class DagListener;
class NetworkingApi;

class UnixFsPathResolver;

class BlockStorage {
 public:
  BlockStorage();

  BlockStorage(BlockStorage const&) = delete;

  ~BlockStorage() noexcept;

  bool Store(std::string headers, Block&& block);
  bool Store(std::string const& cid, std::string headers, std::string body);
  bool Store(std::string cid_str,
             Cid const& cid,
             std::string headers,
             std::string body);
  bool Store(Cid const& cid, std::string headers, Block&&);
  bool Store(std::string cid_str, Cid const& cid, std::string headers, Block&&);

  Block const* Get(std::string const& cid) const;

  void AddListening(UnixFsPathResolver*);

  void StopListening(UnixFsPathResolver*);

  void CheckListening();

  void cache_search_initiator(std::function<void(std::string)>);

 private:
  std::size_t index_ = 0UL;
  std::array<Block, 128> nodes_;
  flat_map<std::string, Block*> cid2node_;
  flat_set<UnixFsPathResolver*> listening_;
  std::function<void(std::string)> cache_search_initiator_ = {};
};
}  // namespace ipfs

#endif  // IPFS_BLOCKS_H_
