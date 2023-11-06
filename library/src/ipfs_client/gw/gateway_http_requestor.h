#ifndef IPFS_GATEWAY_HTTP_REQUESTOR_H_
#define IPFS_GATEWAY_HTTP_REQUESTOR_H_

#include <ipfs_client/gw/requestor.h>
#include <ipfs_client/ipld/dag_node.h>

#include <set>
#include <string>

namespace ipfs::gw {
class GatewayHttpRequestor final : public Requestor {
  std::string prefix_;
  std::set<std::string> seen_;
  std::set<std::string> aff_good_, aff_bad_;
  std::set<Type> typ_good_, typ_bad_;
  std::size_t pending = 0UL;

  HandleOutcome handle(RequestPtr) override;
  std::string_view name() const override;
  using Cid = libp2p::multi::ContentIdentifier;
  ipfs::ipld::NodePtr node_from_type(std::optional<Cid> const& cid,
                                     ipfs::gw::Type,
                                     std::string_view body) const;
  int target(GatewayRequest const&) const;

 public:
  GatewayHttpRequestor(std::string gateway_prefix, std::shared_ptr<ContextApi>);
  ~GatewayHttpRequestor() noexcept override;
};
}  // namespace ipfs::gw

#endif  // IPFS_GATEWAY_HTTP_REQUESTOR_H_
