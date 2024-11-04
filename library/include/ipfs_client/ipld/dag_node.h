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

/*! Response when the partition does not yet have all the blocks
 *  needed to fulfill the request.
 */
struct MoreDataNeeded {
  /*! Construct if just one piece of data is known to be needed
   *  @param one The /ipfs/path of the data needed
   */
  explicit MoreDataNeeded(std::string one) : ipfs_abs_paths_{{one}} {}
  /*! Construct from a range of needed data
   *  @param many The /ipfs/paths known to be needed
   */
  template <class Range>
  explicit MoreDataNeeded(Range const& many)
      : ipfs_abs_paths_(many.begin(), many.end()) {}
  /*! The paths
   */
  std::vector<std::string> ipfs_abs_paths_;
};
enum class ProvenAbsent {};
/*! Response from a redirection or symlink that causes the requested path
 *  to be equivalent to another.
 */
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
  /*! The child nodes of this node, possibly not yet fetched.
   *  @note In some cases the string is empty. For example as stem node of a UnixFS (multi-node) file.
   */
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
  /*! Query the DAG
   *  @param params Information about what you're looking for
   *  @return The data needed
   *    or how to get it
   *    or some additional info needed before asking again.
   */
  ResolveResult Resolve(ResolutionState& params);

  static NodePtr fromBytes(std::shared_ptr<Client> const& api,
                           Cid const&,
                           ByteView bytes);
  static NodePtr fromBytes(std::shared_ptr<Client> const& api,
                           Cid const&,
                           std::string_view bytes);
  static NodePtr fromBlock(PbDag const&);

  virtual ~DagNode() noexcept;

  /*! Get a root node that is logically this node
   * @return this iff this is of type ipfs::ipld::Root
   *    Otherwise a Root that has this as a child.
   */
  virtual NodePtr rooted();
  /*! @return *this if it's not a Root, otherwise its child
   */
  virtual NodePtr deroot();

  /*! @defgroup downcast Down-casting to specific subclasses
   *  @brief I wish I had access to dynamic_cast, but Chromium.
   *  @{
   */

  /*! @return this if this is a DNSLinkName, otherwise nullptr
   */
  virtual DnsLinkName const* as_dnslink() const { return nullptr; }
  /*! @return this if this is a DirShard, otherwise nullptr
   */
  virtual DirShard* as_hamt() { return nullptr; }
  /*! @return this if this is a IpnsName, otherwise nullptr
   */
  virtual IpnsName const* as_ipns() const { return nullptr; }

  /** @} */

  /*! @return Whether the node should be considered expired.
   *  @note immutable data never expires
   */
  virtual bool expired() const;
  /*! @return Whether this is a "better"/more-up-to-date replacement
   *  @param another The node to compare to.
   */
  virtual bool PreferOver(DagNode const& another) const;

  /*! Provide the Client API to be used in DAG operations
   *  @param api Shared pointer to API for the context being run in
   */
  void set_api(std::shared_ptr<Client> api);
  void source(BlockSource src) { source_ = src; }
};
}  // namespace ipfs::ipld

std::ostream& operator<<(std::ostream&, ipfs::ipld::PathChange const&);

#endif  // IPFS_DAG_NODE_H_
