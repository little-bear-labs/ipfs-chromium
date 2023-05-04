#ifndef IPFS_NODE_HELPER_H_
#define IPFS_NODE_HELPER_H_

#include <ipfs_client/dag_block.h>
#include <ipfs_client/networking_api.h>

#include <vocab/raw_ptr.h>

#include <memory>
#include <string>

namespace ipfs {
class BlockStorage;
class DagListener;
class UnixFsPathResolver;
namespace unix_fs {
class NodeHelper {
 public:
  static std::unique_ptr<NodeHelper> FromBlockType(Block::Type,
                                                   std::string path_element);

  virtual bool Process(std::unique_ptr<NodeHelper>&,
                       std::shared_ptr<DagListener>,
                       std::function<void(std::string, Priority)> requestor,
                       std::string& target_cid) = 0;

  void cid(std::string const& val) { cid_ = val; }
  void storage(BlockStorage& val);
  void resolver(UnixFsPathResolver& val) { resolver_ = &val; }
  void api(NetworkingApi& val) { api_ = &val; }

  virtual ~NodeHelper() noexcept;

 protected:
  std::string cid_;
  raw_ptr<BlockStorage> storage_ = nullptr;
  raw_ptr<UnixFsPathResolver> resolver_ = nullptr;
  raw_ptr<NetworkingApi> api_ = nullptr;

  Block const* block() const;
  void Delegate(NodeHelper&) const;
};
}  // namespace unix_fs
}  // namespace ipfs

#endif  // IPFS_NODE_HELPER_H_
