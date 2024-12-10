#ifndef IPFS_DAG_JSON_NODE_H_
#define IPFS_DAG_JSON_NODE_H_

#include <ipfs_client/dag_json_value.h>
#include <ipfs_client/ipld/dag_node.h>

namespace ipfs::ipld {
/*! A DAG-JSON value.
 *  @details
 *    The `links_` are path-able, so...
 *    * If it represents a dictionary, the links are the members of the dict.
 *    * If it's a "link", the node has exactly one child, the thing linked to.
 *    * Otherwise the one node contains all child JSON values.
 *      - e.g. if it's an array this one node contains all members of the array
 */
class DagJsonNode final : public DagNode {
  std::unique_ptr<DagJsonValue> data_;
  std::string html_;
  ResolveResult resolve(ResolutionState& params) override;
  Link* is_link();
  std::string const& html();

 public:
  /*! Construct from parsed JSON
   */
  explicit DagJsonNode(std::unique_ptr<DagJsonValue>);
  ~DagJsonNode() noexcept override;
};

}  // namespace ipfs::ipld

#endif  // IPFS_DAG_JSON_NODE_H_
