#ifndef IPFS_UNIXFS_PATH_RESOLVER_H_
#define IPFS_UNIXFS_PATH_RESOLVER_H_

#include "dag_block.h"
#include "dag_listener.h"
#include "scheduler.h"

#include <functional>
#include <memory>
#include <string>

namespace ipfs {

class BlockStorage;
class NetworkingApi;
class GeneratedDirectoryListing;

namespace unix_fs {
class NodeHelper;
}

class UnixFsPathResolver {
 public:
  UnixFsPathResolver(BlockStorage&,
                     std::string cid,
                     std::string path,
                     std::shared_ptr<NetworkingApi>);
  ~UnixFsPathResolver() noexcept;

  void Step(std::shared_ptr<DagListener>);
  std::shared_ptr<DagListener> MaybeGetPreviousListener();
  std::string const& current_cid() const;
  std::string const& original_path() const;
  Priority priority() const { return prio_; }
  std::vector<std::string> const& involved_cids() { return involved_cids_; }

 private:
  BlockStorage& storage_;
  std::string cid_;
  std::string path_;
  std::string original_path_;
  std::shared_ptr<NetworkingApi> api_;
  std::unique_ptr<unix_fs::NodeHelper> helper_;
  std::weak_ptr<DagListener> old_listener_;
  Priority prio_ = 9;
  struct PriorReq {
    Priority prio;
    std::time_t when;
  };
  std::unordered_map<std::string, PriorReq> already_requested_;
  std::vector<std::string> involved_cids_;

  void Request(std::shared_ptr<DagListener>&, std::string const& cid, Priority);
  void GetHelper(Block::Type);
  std::string pop_path();
};

}  // namespace ipfs

#endif  // IPFS_UNIXFS_PATH_RESOLVER_H_
