#ifndef IPFS_SYMLINK_H_
#define IPFS_SYMLINK_H_

#include <ipfs_client/ipld/dag_node.h>

namespace ipfs::ipld {
class Symlink : public DagNode {
  std::string const target_;

  ResolveResult resolve(ResolutionState& params) override;

  bool is_absolute() const;

 public:
  Symlink(std::string target);
  ~Symlink() noexcept override;
};
}  // namespace ipfs::ipld

#endif  // IPFS_SYMLINK_H_
