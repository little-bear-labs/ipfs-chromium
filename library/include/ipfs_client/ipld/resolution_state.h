#ifndef IPFS_RESOLUTION_STATE_H_
#define IPFS_RESOLUTION_STATE_H_

#include <vocab/slash_delimited.h>

#include <functional>
#include <memory>

namespace ipfs {
class ContextApi;
}

namespace ipfs::ipld {
class DagNode;
using NodePtr = std::shared_ptr<DagNode>;
using BlockLookup = std::function<NodePtr(std::string const&)>;

class ResolutionState {
  friend class DagNode;
  std::string resolved_path_components;
  SlashDelimited unresolved_path;
  BlockLookup get_available_block;

 public:
  SlashDelimited MyPath() const;
  SlashDelimited PathToResolve() const;
  bool IsFinalComponent() const;
  std::string NextComponent(ContextApi const*) const;
  NodePtr GetBlock(std::string const& block_key) const;

  ResolutionState WithPath(std::string_view) const;
  ResolutionState RestartResolvedPath() const;
};
}  // namespace ipfs::ipld

#endif  // IPFS_RESOLUTION_STATE_H_
