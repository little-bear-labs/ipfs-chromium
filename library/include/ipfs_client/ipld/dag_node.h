#ifndef IPFS_DAG_NODE_H_
#define IPFS_DAG_NODE_H_

#include "link.h"
#include "resolution_state.h"

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipld/block_source.h>

#include <ipfs_client/cid.h>
#include <ipfs_client/response.h>
#include <vocab/slash_delimited.h>

#include <cstdint>

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace ipfs {
class PbDag;
class Client;
struct ValidatedIpns;
}  // namespace ipfs
namespace libp2p::multi {
struct ContentIdentifier;
}
namespace ipfs::ipld {

using NodePtr = std::shared_ptr<DagNode>;
class DirShard;
class DnsLinkName;
class IpnsName;

struct MoreDataNeeded {
  MoreDataNeeded(std::string one) : ipfs_abs_paths_{{one}} {}
  template <class Range>
  MoreDataNeeded(Range const& many)
      : ipfs_abs_paths_(many.begin(), many.end()) {}
  std::vector<std::string> ipfs_abs_paths_;
  bool insist_on_car = false;
};
enum class ProvenAbsent {};
struct PathChange {
  std::string new_path;
};

using ResolveResult =
    std::variant<MoreDataNeeded, Response, ProvenAbsent, PathChange>;
/**
 * @brief A block, an IPNS record, etc.
 */
class DagNode : public std::enable_shared_from_this<DagNode> {
  Link* FindChild(std::string_view);
  BlockSource source_;

  virtual ResolveResult resolve(ResolutionState& params) = 0;

 protected:
  std::vector<std::pair<std::string, Link>> links_;
  std::shared_ptr<Client> api_;

  ///< When the next path element is what's needed, and it should already be a
  ///< link known about...
  ResolveResult CallChild(ResolutionState&);

  ///< As before, but it might be possible to create on the fly if not known
  ResolveResult CallChild(ResolutionState&,
                          std::function<NodePtr(std::string_view)> gen_child);

  ///< When the child's name is not the next element in the path, but it must be
  ///< known about. e.g. index.html for a path ending in a directory
  ResolveResult CallChild(ResolutionState&, std::string_view link_key);

  ///< Add the link if not present, then CallChild(ResolutionState)
  ResolveResult CallChild(ResolutionState&,
                          std::string_view link_key,
                          std::string_view block_key);
  
 public:
  // ResolveResult resolve(SlashDelimited initial_path, BlockLookup);
  ResolveResult Resolve(ResolutionState& params);

  static NodePtr fromBytes(std::shared_ptr<Client> const& api,
                           Cid const&,
                           ByteView bytes);
  static NodePtr fromBytes(std::shared_ptr<Client> const& api,
                           Cid const&,
                           std::string_view bytes);
  static NodePtr fromBlock(PbDag const&);

  virtual ~DagNode() noexcept {}

  virtual NodePtr rooted();
  virtual NodePtr deroot();

  // Wish I had access to dynamic_cast
  virtual DnsLinkName const* as_dnslink() const { return nullptr; }
  virtual DirShard* as_hamt() { return nullptr; }
  virtual IpnsName const* as_ipns() const { return nullptr; }

  virtual bool expired() const;
  virtual bool PreferOver(DagNode const& another) const;

  void set_api(std::shared_ptr<Client>);
  void source(BlockSource src) { source_ = src; }
};
}  // namespace ipfs::ipld

std::ostream& operator<<(std::ostream&, ipfs::ipld::PathChange const&);

#endif  // IPFS_DAG_NODE_H_
