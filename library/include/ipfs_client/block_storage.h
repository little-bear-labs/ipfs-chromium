#ifndef IPFS_BLOCKS_H_
#define IPFS_BLOCKS_H_

#include "dag_block.h"
#include "vocab/flat_mapset.h"

#include <list>
#include <string>
#include <string_view>

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

  Block const* Get(std::string const& cid, bool deep = true);
  std::string const* GetHeaders(std::string const& cid);

  void AddListening(UnixFsPathResolver*);

  void StopListening(UnixFsPathResolver*);

  void CheckListening();

  void cache_search_initiator(std::function<void(std::string)>);

 private:
  struct Record {
    Record();
    ~Record() noexcept;
    std::time_t last_access = 0;
    std::string cid_str = {};
    Block block = {};
    std::string headers = {};
  };
  std::list<Record> records_;
  using Iter = decltype(records_)::iterator;
  Iter reuse_ = records_.end();
  flat_map<std::string, Record*> cid2record_;
  flat_set<UnixFsPathResolver*> listening_;
  flat_set<std::string> checking_;
  std::function<void(std::string)> cache_search_initiator_ = {};

  Record const* GetInternal(std::string const&, bool);
  Record* FindFree(std::time_t);
  Record* Allocate();
};
}  // namespace ipfs

#endif  // IPFS_BLOCKS_H_
