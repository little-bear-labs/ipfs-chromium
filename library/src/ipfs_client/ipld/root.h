#ifndef IPFS_ROOT_H_
#define IPFS_ROOT_H_

#include <ipfs_client/ipld/dag_node.h>
#include <ipfs_client/redirects.h>

#include <optional>

namespace ipfs::ipld {
class Root : public DagNode {
  std::optional<redirects::File> redirects_;

  ResolveResult resolve(ResolutionState& params) override;
  std::shared_ptr<DagNode> rooted() override;
  std::shared_ptr<DagNode> deroot() override;
  bool expired() const override;

 public:
  explicit Root(std::shared_ptr<DagNode>);
  ~Root() noexcept override;
};
}  // namespace ipfs::ipld

#endif  // IPFS_ROOT_H_
