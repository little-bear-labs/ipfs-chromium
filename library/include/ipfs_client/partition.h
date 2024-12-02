#ifndef IPFS_ORCHESTRATOR_H_
#define IPFS_ORCHESTRATOR_H_

#include "ipfs_client/ipld/dag_node.h"

#include <ipfs_client/gw/requestor.h>
#include <vocab/flat_mapset.h>

#include <memory>
#include <string>

namespace ipfs {

class Client;
/*! A partition of your in-memory stored IPFS DAGs
 *  Potentially useful in the fight against cache timing attacks
 */
class Partition : public std::enable_shared_from_this<Partition> {
 public:
  using MimeDetection = std::function<
      std::string(std::string, std::string_view, std::string const&)>;
  void build_response(std::shared_ptr<IpfsRequest>);
  bool add_node(std::string key, ipld::NodePtr);
  bool has_key(std::string const& k) const;
  std::size_t Stored() const { return dags_.size(); }

  explicit Partition(std::shared_ptr<gw::Requestor> requestor,
                     std::shared_ptr<Client>);

 private:
  flat_map<std::string, ipld::NodePtr> dags_;
  std::shared_ptr<Client> api_;
  std::shared_ptr<gw::Requestor> requestor_;

  void from_tree(std::shared_ptr<IpfsRequest>,
                 ipld::NodePtr&,
                 SlashDelimited,
                 std::string const&);
  bool gw_request(std::shared_ptr<IpfsRequest>,
                  SlashDelimited path,
                  std::string const& aff);
  std::string sniff(SlashDelimited, std::string const&) const;
};
}  // namespace ipfs::ipld

#endif  // IPFS_ORCHESTRATOR_H_
