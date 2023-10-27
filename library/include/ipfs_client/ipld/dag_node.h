#ifndef IPFS_DAG_NODE_H_
#define IPFS_DAG_NODE_H_

#include "link.h"

#include <ipfs_client/gw/gateway_request.h>
#include "ipfs_client/response.h"

#include <vocab/slash_delimited.h>

#include <cstdint>

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace ipfs {
class Block;
struct ValidatedIpns;
}

namespace ipfs::ipld {

class DagNode;
using NodePtr = std::shared_ptr<DagNode>;
class DirShard;

struct MoreDataNeeded {
  std::vector<std::string> ipfs_abs_paths_;
};
enum class ProvenAbsent {};

using ResolveResult = std::variant<MoreDataNeeded, Response, ProvenAbsent>;
/**
 * @brief A block, an IPNS record, etc.
 */
class DagNode : public std::enable_shared_from_this<DagNode> {
 protected:
  std::vector<std::pair<std::string, Link>> links_;

 public:
  using BlockLookup = std::function<NodePtr(std::string const&)>;
  virtual ResolveResult resolve(SlashDelimited path,
                                BlockLookup,
                                std::string& up_to_here) = 0;

  virtual NodePtr rooted();
  virtual NodePtr deroot();
  virtual DirShard* as_hamt();  // Wish I had access to dynamic_cast

  static NodePtr fromBlock(Block const&);
  static NodePtr fromIpnsRecord(ValidatedIpns const&);
};
}  // namespace ipfs::ipld

#endif  // IPFS_DAG_NODE_H_
