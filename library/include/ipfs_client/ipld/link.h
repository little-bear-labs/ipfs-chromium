#ifndef IPFS_LINK_H_
#define IPFS_LINK_H_

#include <memory>
#include <string>

namespace ipfs::ipld {

class DagNode;
using Ptr = std::shared_ptr<DagNode>;

class Link {
 public:
  std::string cid;
  Ptr node;

  Link(std::string);
  explicit Link(std::string, std::shared_ptr<DagNode>);
};
}  // namespace ipfs::ipld

#endif  // IPFS_LINK_H_
