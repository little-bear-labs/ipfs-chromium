#ifndef IPFS_UNIXFS_PATH_RESOLVER_H_
#define IPFS_UNIXFS_PATH_RESOLVER_H_

#include <functional>
#include <memory>
#include <string>

namespace ipfs {

class BlockStorage;

class UnixFsPathResolver : std::enable_shared_from_this<UnixFsPathResolver> {
 public:
  using RequestByCid = std::function<void(std::string const&)>;
  using FileContentReceiver = std::function<void(std::string const&)>;
  using NullanaryHook = std::function<void()>;
  UnixFsPathResolver(BlockStorage&,
                     std::string cid,
                     std::string path,
                     RequestByCid request_required,
                     RequestByCid request_prefetch,
                     FileContentReceiver receive_bytes,
                     NullanaryHook on_complete);
  ~UnixFsPathResolver() noexcept;

  std::string const& waiting_on() const { return cid_; }
  void Step(std::shared_ptr<UnixFsPathResolver>);

 private:
  BlockStorage& storage_;
  std::string cid_;
  std::string path_;
  RequestByCid request_required_;
  RequestByCid request_prefetch_;
  FileContentReceiver receive_bytes_;
  NullanaryHook on_complete_;
  std::string written_through_ = {};
};
}  // namespace ipfs
#endif  // IPFS_UNIXFS_PATH_RESOLVER_H_
