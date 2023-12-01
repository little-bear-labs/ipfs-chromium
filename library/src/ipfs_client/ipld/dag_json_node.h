#ifndef IPFS_DAG_JSON_NODE_H_
#define IPFS_DAG_JSON_NODE_H_

#include <ipfs_client/dag_json_value.h>
#include <ipfs_client/ipld/dag_node.h>

namespace ipfs::ipld {
class DagJsonNode final : public DagNode {
  std::unique_ptr<DagJsonValue> data_;
  std::string html_;
  ResolveResult resolve(SlashDelimited path,
                        BlockLookup,
                        std::string& up_to_here) override;
  Link* is_link();
  ResolveResult child_resolve(SlashDelimited path,
                              BlockLookup,
                              std::string& up_to_here,
                              Link& l);
  std::string const& html();

 public:
  DagJsonNode(std::unique_ptr<DagJsonValue>);
  ~DagJsonNode() noexcept override;
};

}  // namespace ipfs::ipld

#endif  // IPFS_DAG_JSON_NODE_H_
