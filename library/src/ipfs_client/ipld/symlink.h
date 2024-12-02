#ifndef IPFS_SYMLINK_H_
#define IPFS_SYMLINK_H_

#include <ipfs_client/ipld/dag_node.h>

namespace ipfs::ipld {
class Symlink : public DagNode {
  std::string const target_;

  ResolveResult resolve(ResolutionState& params) override;

  bool is_absolute() const;

 public:
  /*! Construct from the text of the target
   *  @target Either a path relative to this node's,
   *    OR /an/absolute/path which is relative to the DAG root
   *  @note If it points outside the DAG it'll be considered broken.
   *    This can be achieved by simply using too many ../../..
   */
  explicit Symlink(std::string target);
  ~Symlink() noexcept override;
};
}  // namespace ipfs::ipld

#endif  // IPFS_SYMLINK_H_
