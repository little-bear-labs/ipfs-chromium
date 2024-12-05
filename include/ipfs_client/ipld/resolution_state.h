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
  /*! Construct from starting info
   *  @param path_to_resolve Relative to the node resolve is being called on
   *  @param semantic If the resolution results in a stem, i.e. a directory-like not a file,
   *    would the preference be for a listing preview HTML page -OR-
   *    JSON that describes the links to files and CIDs to merge in with this JSON (in the case of a sharded dir)
   *  @param lookup Functor that returns a pointer to a node for a given CID if it's already availabe, nullptr otherwise.
   */
  ResolutionState(SlashDelimited path_to_resolve, ResponseSemantic semantic, BlockLookup lookup);

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
