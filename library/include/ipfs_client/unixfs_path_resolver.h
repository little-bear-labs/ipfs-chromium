#ifndef IPFS_UNIXFS_PATH_RESOLVER_H_
#define IPFS_UNIXFS_PATH_RESOLVER_H_

#include "block.h"

#include <functional>
#include <memory>
#include <string>

namespace ipfs {

class BlockStorage;

class UnixFsPathResolver
    : public std::enable_shared_from_this<UnixFsPathResolver> {
 public:
  using RequestByCid = std::function<void(std::string const&)>;
  using FileContentReceiver = std::function<void(std::string const&)>;
  using CompetionHook = std::function<void(std::string)>;
  UnixFsPathResolver(BlockStorage&,
                     std::string cid,
                     std::string path,
                     RequestByCid request_required,
                     RequestByCid request_prefetch,
                     FileContentReceiver receive_bytes,
                     CompetionHook on_complete);
  ~UnixFsPathResolver() noexcept;

  std::string const& waiting_on() const { return cid_; }
  void Step(std::shared_ptr<UnixFsPathResolver>);

 private:
  BlockStorage& storage_;
  std::string cid_;
  std::string path_;
  std::string original_path_;
  RequestByCid request_required_;
  RequestByCid request_prefetch_;
  FileContentReceiver receive_bytes_;
  CompetionHook on_complete_;
  std::string written_through_ = {};
  bool force_type_dir_ = false;

  void CompleteDirectory(Block const&);
};
}  // namespace ipfs
#endif  // IPFS_UNIXFS_PATH_RESOLVER_H_
