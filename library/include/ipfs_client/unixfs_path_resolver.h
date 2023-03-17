#ifndef IPFS_UNIXFS_PATH_RESOLVER_H_
#define IPFS_UNIXFS_PATH_RESOLVER_H_

#include "block.h"
#include "dag_listener.h"
#include "scheduler.h"

#include <functional>
#include <memory>
#include <string>

namespace ipfs {

class BlockStorage;
class FrameworkApi;
class GeneratedDirectoryListing;
class Scheduler;

class UnixFsPathResolver {
 public:
  UnixFsPathResolver(BlockStorage&,
                     Scheduler&,
                     std::string cid,
                     std::string path,
                     std::shared_ptr<FrameworkApi>);
  ~UnixFsPathResolver() noexcept;

  std::string const& waiting_on() const;
  std::string describe() const;
  void Step(std::shared_ptr<DagListener>);

 private:
  BlockStorage& storage_;
  Scheduler& sched_;
  std::string cid_;
  std::string path_;
  std::string original_cid_;
  std::string original_path_;
  std::string written_through_ = {};
  std::string awaiting_;  // TODO dehack
  bool force_type_dir_ = false;
  std::unordered_map<std::string, std::pair<Scheduler::Priority, std::time_t>>
      already_requested_;
  std::vector<std::string> hamt_hexs_;
  std::string head_;
  std::shared_ptr<FrameworkApi> api_;

  void CompleteDirectory(std::shared_ptr<DagListener>&, Block const&);
  void ProcessDirectory(std::shared_ptr<DagListener>&, Block const&);
  void ProcessLargeFile(std::shared_ptr<DagListener>&, Block const&);
  void ProcessDirShard(std::shared_ptr<DagListener>&, Block const&);
  void Request(std::shared_ptr<DagListener>&,
               std::string const& cid,
               Scheduler::Priority);
  std::string GuessContentType(std::string_view content);

  void RequestHamtDescendents(std::shared_ptr<DagListener>,
                              bool&,
                              Block const& block,
                              unsigned) const;
  bool ListHamt(std::shared_ptr<DagListener>&,
                Block const&,
                GeneratedDirectoryListing&,
                unsigned);
  void ResolveByHashBits(Block const&,
                         std::uint64_t,
                         unsigned,
                         std::string_view,
                         unsigned,
                         std::shared_ptr<FrameworkApi>&);
};
}  // namespace ipfs
#endif  // IPFS_UNIXFS_PATH_RESOLVER_H_
