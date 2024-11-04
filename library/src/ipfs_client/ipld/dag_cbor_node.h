#ifndef IPFS_DAG_CBOR_NODE_H_
#define IPFS_DAG_CBOR_NODE_H_

#include <ipfs_client/ipld/dag_node.h>

#include <ipfs_client/client.h>

namespace ipfs::ipld {
/*! A DAG-CBOR dict
 */
class DagCborNode final : public DagNode {
  ResolveResult resolve(ResolutionState&) override;

 public:
  /*! The dict or document being represented
   */
  using Data = DagCborValue;
  explicit DagCborNode(std::unique_ptr<Data>);
  ~DagCborNode() noexcept override;

 private:
  std::unique_ptr<Data> doc_;
};
}

#endif  // IPFS_DAG_CBOR_NODE_H_
