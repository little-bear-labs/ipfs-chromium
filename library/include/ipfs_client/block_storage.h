#ifndef IPFS_BLOCKS_H_
#define IPFS_BLOCKS_H_

#include "dag_block.h"
#include "vocab/flat_mapset.h"

#include <list>
#include <string>
#include <string_view>

namespace ipfs {
class DagListener;
class ContextApi;

class UnixFsPathResolver;

class BlockStorage {
 public:
  BlockStorage();

  BlockStorage(BlockStorage const&) = delete;

  ~BlockStorage() noexcept;

  bool Store(std::string headers, std::string const& body, Block&& block);
  bool Store(std::string const& cid, std::string headers, std::string body);
  bool Store(std::string cid_str,
             Cid const& cid,
             std::string headers,
             std::string body);
  bool Store(Cid const& cid,
             std::string headers,
             std::string const& body,
             Block&&);
  bool Store(std::string cid_str,
             Cid const& cid,
             std::string headers,
             std::string const& body,
             Block&&);

  Block const* Get(std::string const& cid, bool deep = true);
  std::string const* GetHeaders(std::string const& cid);

  void AddListening(UnixFsPathResolver*);

  void StopListening(UnixFsPathResolver*);

  void CheckListening();

  using SerializedStorageHook =
      std::function<void(std::string, std::string, std::string)>;
  void AddStorageHook(SerializedStorageHook);

 private:
  struct Record {
    Record();
    ~Record() noexcept;
    std::time_t last_access = 0L;
    std::string cid_str = {};
    Block block = {};
    std::string headers = {};
  };
  std::list<Record> records_ = std::list<Record>(0xFFUL);
  using Iter = decltype(records_)::iterator;
  flat_map<std::string, Record*> cid2record_;
  flat_set<UnixFsPathResolver*> listening_;
  bool checking_ = false;
  std::vector<SerializedStorageHook> hooks_;

  Record const* GetInternal(std::string const&);
  Record* FindFree(std::time_t);
  Record* Allocate();
};
}  // namespace ipfs

#endif  // IPFS_BLOCKS_H_
