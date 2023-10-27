#ifndef IPFS_ROOT_H_
#define IPFS_ROOT_H_

#include <ipfs_client/ipld/dag_node.h>
#include <ipfs_client/redirects.h>

#include <optional>

namespace ipfs::ipld {
class Root : public DagNode {
  std::optional<redirects::File> redirects_;

  ResolveResult resolve(SlashDelimited path,
                        BlockLookup,
                        std::string&) override;
  std::shared_ptr<DagNode> rooted() override;
  std::shared_ptr<DagNode> deroot() override;

 public:
  Root(std::shared_ptr<DagNode>);
  virtual ~Root() noexcept;
};
}  // namespace ipfs::ipld

#endif  // IPFS_ROOT_H_
