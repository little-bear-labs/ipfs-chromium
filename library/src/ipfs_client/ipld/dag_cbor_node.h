#ifndef IPFS_DAG_CBOR_NODE_H_
#define IPFS_DAG_CBOR_NODE_H_

#include <ipfs_client/ipld/dag_node.h>

#include <ipfs_client/context_api.h>

namespace ipfs::ipld {
class DagCborNode final : public DagNode {
  ResolveResult resolve(SlashDelimited,BlockLookup,
                        std::string&) override;

 public:
  using Data = DagCborValue;
  explicit DagCborNode(std::unique_ptr<Data>);
  ~DagCborNode() noexcept override;

 private:
  std::unique_ptr<Data> doc_;
};
}

#endif  // IPFS_DAG_CBOR_NODE_H_
