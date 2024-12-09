#ifndef IPFS_ROOT_H_
#define IPFS_ROOT_H_

#include <ipfs_client/ipld/dag_node.h>
#include <ipfs_client/redirects.h>

#include <optional>

namespace ipfs::ipld {
/*! Since some operations will behave differently if a given node is being treated as
   *    a DAG root, this proxy wraps around a node to give it those properties
   */
class Root : public DagNode {
  std::optional<redirects::File> redirects_;

  ResolveResult resolve(ResolutionState& params) override;
  std::shared_ptr<DagNode> rooted() override;
  std::shared_ptr<DagNode> deroot() override;
  bool expired() const override;

 public:
  /*! Construct
   *  @param node The actual node being treated as a DAG root
   */
  explicit Root(std::shared_ptr<DagNode> node);
  ~Root() noexcept override;
};
}  // namespace ipfs::ipld

#endif  // IPFS_ROOT_H_
