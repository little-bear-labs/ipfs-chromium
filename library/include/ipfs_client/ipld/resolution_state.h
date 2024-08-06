#ifndef IPFS_RESOLUTION_STATE_H_
#define IPFS_RESOLUTION_STATE_H_

#include <ipfs_client/ipld/dag_headers.h>
#include <ipfs_client/response_semantic.h>

#include <vocab/slash_delimited.h>

#include <functional>
#include <memory>

namespace ipfs {
class Client;
}

namespace ipfs::ipld {
class DagNode;
using NodePtr = std::shared_ptr<DagNode>;
using BlockLookup = std::function<NodePtr(std::string const&)>;

class ResolutionState {
  std::string resolved_path_components;
  SlashDelimited unresolved_path;
  ResponseSemantic semantic_;
  BlockLookup get_available_block;

 public:
  ResolutionState(SlashDelimited path_to_resolve, ResponseSemantic, BlockLookup);

  SlashDelimited MyPath() const;
  SlashDelimited PathToResolve() const;
  bool IsFinalComponent() const;
  std::string NextComponent(Client*) const;
  NodePtr GetBlock(std::string const& block_key) const;

  ResolutionState WithPath(std::string_view) const;
  ResolutionState RestartResolvedPath() const;
  ResponseSemantic Semantic() const;

  void Descend();

  ipld::DagHeaders headers;
};
}  // namespace ipfs::ipld

#endif  // IPFS_RESOLUTION_STATE_H_
