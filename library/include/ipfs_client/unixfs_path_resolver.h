#ifndef IPFS_UNIXFS_PATH_RESOLVER_H_
#define IPFS_UNIXFS_PATH_RESOLVER_H_

#include "dag_block.h"
#include "dag_listener.h"
#include "scheduler.h"

#include <functional>
#include <memory>
#include <string>

namespace ipfs {

class BlockRequestor;
class BlockStorage;
class ContextApi;
class GeneratedDirectoryListing;

namespace unix_fs {
class NodeHelper;
}

/*!
 * \brief Asynchronously descend a UnixFS dag given a root CID and a path.
 *    Get the file the combo refers to.
 */
class UnixFsPathResolver {
 public:
  /*!
   * \brief Create a new UnixFsPathResolver
   * \param stor - block storage to fetch existing blocks from and into which
   *    new blocks are stored
   * \param req  - The object used to request blocks we don't have
   * \param cid  - The root CID of the dag in question
   * \param path - The path down the dag being resolved
   */
  UnixFsPathResolver(BlockStorage& stor,
                     BlockRequestor& req,
                     std::string cid,
                     std::string path,
                     std::shared_ptr<ContextApi> api);
  ~UnixFsPathResolver() noexcept;

  /*!
   * \brief Attempt to move closer to resolution
   * \param dl - The listener waiting for resolution
   * \pre   dl is not null
   */
  void Step(std::shared_ptr<DagListener> dl);

  /*!
   * \brief  Fetch the listener previously passed to UnixFsPathResolver::Step
   * \note   This class does not manage or extend the lifetime.
   * \return most recent listener, or null if not available
   */
  std::shared_ptr<DagListener> MaybeGetPreviousListener();

  /*!
   * \brief  The CID of the block currently being waited upon
   * \return MB-MF string representation of the CID
   */
  std::string const& current_cid() const;

  /*!
   * \brief Simple getter
   * \return the `path` argument to the constructor
   */
  std::string const& original_path() const;

  /*!
   * \brief Simple getter
   * \return the current priority for generated requests
   */
  Priority priority() const { return prio_; }

  /*!
   * \brief  All CIDs that were parsed to resolve the path
   * \return list of MB-MF string representations of the CIDs
   * \note   The root CID may be from the constructor, but others are from links
   *    in the nodes traversed. They may be of various forms, including CIDv0.
   */
  std::vector<std::string> const& involved_cids() { return involved_cids_; }

 private:
  BlockStorage& storage_;
  BlockRequestor& requestor_;
  std::string cid_;
  std::string path_;
  std::string original_path_;
  std::shared_ptr<ContextApi> api_;
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
