#ifndef IPFS_IPFS_BLOCK_CACHE_H_
#define IPFS_IPFS_BLOCK_CACHE_H_

#include <ipfs_client/dag_block.h>

#include <base/component_export.h>
#include <net/base/io_buffer.h>
#include <net/disk_cache/disk_cache.h>

#include <memory>

namespace disk_cache {
class Backend;
}

namespace ipfs {
class BlockStorage;

class COMPONENT_EXPORT(IPFS) BlockCache {
  std::unique_ptr<disk_cache::Backend> mem_;
  std::unique_ptr<disk_cache::Backend> dsk_;

 public:
  BlockCache();
  ~BlockCache() noexcept;

  void Load(std::string cid, BlockStorage*);
  void Store(std::string cid, std::string headers, std::string block_bytes);

 private:
  disk_cache::Backend* mem();
  disk_cache::Backend* dsk();
  std::unordered_set<std::string> known_absent_;
  std::unordered_set<std::string> disk_load_pending_;
  bool disk_cache_pending_ = false;

  void OnDiskOpen(std::string, BlockStorage*, disk_cache::EntryResult);
  void OnDiskHeader(std::string,
                    BlockStorage*,
                    scoped_refptr<net::IOBufferWithSize>,
                    std::shared_ptr<disk_cache::Entry>,
                    int);
  void OnDiskBody(std::string,
                  BlockStorage*,
                  scoped_refptr<net::IOBufferWithSize>,
                  std::shared_ptr<disk_cache::Entry>,
                  std::string,
                  int);
  static void Store(std::string_view,
                    disk_cache::Backend*,
                    std::string cid,
                    std::string headers,
                    std::string block_bytes);
};
}  // namespace ipfs

#endif  // IPFS_IPFS_BLOCK_CACHE_H_
