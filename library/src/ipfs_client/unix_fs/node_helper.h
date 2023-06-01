#ifndef IPFS_NODE_HELPER_H_
#define IPFS_NODE_HELPER_H_

#include <ipfs_client/block_requestor.h>
#include <ipfs_client/dag_block.h>

#include <vocab/raw_ptr.h>

#include <memory>
#include <string>

namespace ipfs {

class BlockStorage;
class ContextApi;
class DagListener;
class UnixFsPathResolver;

namespace unix_fs {

/*!
 * \brief Deal with a block that is a UnixFS node of some sort.
 */
class NodeHelper {
 public:
  /*!
   * \brief Create the appropriate subclass
   * \param typ -
   * \param path_element - if it's a directory-like,
   *    tell it what entry you're trying to resolve
   * \return A subclass instance that might need some more info
   */
  static std::unique_ptr<NodeHelper> FromBlockType(Block::Type typ,
                                                   std::string path_element);

  /*!
   * \brief Get as far in the processing of this block that we can right now
   * \param[out] nh - the next helper if it is knowable
   * \param requestor - a way to request missing blocks
   * \param target_cid[in,out] - The CID of the block currently under
   *    investigation. If being directed to a different block should change.
   * \return Whether target_cid and nh are populated and should be swapped
   * \todo This interface is just bad
   */
  virtual bool Process(std::unique_ptr<NodeHelper>& nh,
                       std::shared_ptr<DagListener> dl,
                       std::function<void(std::string, Priority)> requestor,
                       std::string& target_cid) = 0;

  /*
   * \name Setters
   * call these on a fresh NodeHelper so it knows what it's doing.
   */
  ///@{
  void cid(std::string const& val) { cid_ = val; }  ///< Block/node in question
  void storage(BlockStorage& val);  ///< Storage to interact with

  /// sad that it needs this
  void resolver(UnixFsPathResolver& val) { resolver_ = &val; }
  void api(ContextApi& val) { api_ = &val; }  ///< API for the environment
  ///@{

  virtual ~NodeHelper() noexcept;

 protected:
  std::string cid_;
  raw_ptr<BlockStorage> storage_ = nullptr;
  raw_ptr<UnixFsPathResolver> resolver_ = nullptr;
  raw_ptr<ContextApi> api_ = nullptr;

  Block const* block();
  void Delegate(NodeHelper&) const;
};
}  // namespace unix_fs
}  // namespace ipfs

#endif  // IPFS_NODE_HELPER_H_
