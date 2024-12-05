#ifndef IPFS_LINK_H_
#define IPFS_LINK_H_

#include <memory>
#include <string>

namespace ipfs::ipld {

class DagNode;
using Ptr = std::shared_ptr<DagNode>;

class Link {
 public:
  /*! String representation of the CID
   */
  std::string cid;
  /*! The pointed-to node, or nullptr if not yet resolved
   */
  Ptr node;

  /*! Construct an unresolved IPLD link
   *  @param cid The child's CID
   */
  explicit Link(std::string cid);
  /*! Construct a resolved IPLD link
   *  @param The child's CID
   *  @param node The node linked to if we already have it
   */
  explicit Link(std::string cid, std::shared_ptr<DagNode> node);
};
}  // namespace ipfs::ipld

#endif  // IPFS_LINK_H_
