#ifndef IPFS_GATEWAY_HTTP_REQUESTOR_H_
#define IPFS_GATEWAY_HTTP_REQUESTOR_H_

#include <ipfs_client/gw/requestor.h>
#include <ipfs_client/ipld/dag_node.h>

#include <set>
#include <string>
#include <unordered_map>

namespace ipfs::gw {
class GatewayHttpRequestor final : public Requestor {
  std::string prefix_;
  int strength_;
  std::unordered_map<std::string, int> seen_;
  std::set<std::string> aff_good_, aff_bad_;
  std::set<Type> typ_good_, typ_bad_;
  int pending_ = 0;
  int extra_seconds_ = 0;

  HandleOutcome handle(RequestPtr) override;
  std::string_view name() const override;
  using Cid = libp2p::multi::ContentIdentifier;
  ipfs::ipld::NodePtr node_from_type(std::optional<Cid> const& cid,
                                     ipfs::gw::Type,
                                     std::string_view body) const;
  int target(GatewayRequest const&) const;

 public:
  GatewayHttpRequestor(std::string gateway_prefix,
                       int strength,
                       std::shared_ptr<ContextApi>);
  ~GatewayHttpRequestor() noexcept override;
};
}  // namespace ipfs::gw

#endif  // IPFS_GATEWAY_HTTP_REQUESTOR_H_
