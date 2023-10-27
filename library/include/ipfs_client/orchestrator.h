#ifndef IPFS_ORCHESTRATOR_H_
#define IPFS_ORCHESTRATOR_H_

#include "ipfs_client/ipld/dag_node.h"

#include "vocab/flat_mapset.h"

#include <memory>
#include <string>

namespace ipfs {

class Orchestrator : public std::enable_shared_from_this<Orchestrator> {
  flat_map<std::string, ipld::NodePtr> dags_;

 public:
  using GatewayAccess =
      std::function<void(std::shared_ptr<gw::GatewayRequest>)>;
  using MimeDetection = std::function<
      std::string(std::string, std::string_view, std::string const&)>;
  explicit Orchestrator(GatewayAccess, MimeDetection);
  void build_response(std::shared_ptr<IpfsRequest>);
  void add_node(std::string key, ipld::NodePtr);

 private:
  GatewayAccess gw_requestor_;
  MimeDetection mimer_;
  void from_tree(std::shared_ptr<IpfsRequest>, ipld::NodePtr&, SlashDelimited);
  bool gw_request(std::shared_ptr<IpfsRequest>, SlashDelimited path);
  std::string sniff(SlashDelimited, std::string const&) const;
};
}  // namespace ipfs::ipld

#endif  // IPFS_ORCHESTRATOR_H_
