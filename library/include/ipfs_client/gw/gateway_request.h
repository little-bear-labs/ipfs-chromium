#ifndef IPFS_TRUSTLESS_REQUEST_H_
#define IPFS_TRUSTLESS_REQUEST_H_

#include <ipfs_client/cid.h>
#include <ipfs_client/context_api.h>

#include <vocab/flat_mapset.h>
#include <vocab/slash_delimited.h>

#include <iosfwd>
#include <memory>
#include <optional>
#include <string>

namespace ipfs {
class IpfsRequest;
class Orchestrator;
namespace ipld {
class DagNode;
}
}  // namespace ipfs

namespace ipfs::gw {
class Requestor;

enum class Type : char {
  Block,
  Car,
  Ipns,
  DnsLink,
  Providers,
  Identity,
  Zombie
};
std::string_view name(Type);

constexpr std::size_t BLOCK_RESPONSE_BUFFER_SIZE = 2 * 1024 * 1024;

class GatewayRequest {
  std::shared_ptr<Orchestrator> orchestrator_;
  std::vector<std::function<void(std::string_view)>> bytes_received_hooks;

  void ParseNodes(std::string_view, ContextApi* api);

 public:
  Type type = Type::Zombie;
  std::string main_param;  ///< CID, IPNS name, hostname
  std::string path;        ///< For CAR requests
  std::shared_ptr<IpfsRequest> dependent;
  std::optional<Cid> cid;
  short parallel = 0;
  std::string affinity;
  flat_set<std::string> failures;

  std::string url_suffix() const;
  std::string_view accept() const;
  std::string_view identity_data() const;
  short timeout_seconds() const;
  bool is_http() const;
  std::optional<std::size_t> max_response_size() const;
  std::optional<HttpRequestDescription> describe_http(std::string_view) const;
  std::string debug_string() const;
  void orchestrator(std::shared_ptr<Orchestrator> const&);

  bool RespondSuccessfully(std::string_view,
                           std::shared_ptr<ContextApi> const& api);
  void Hook(std::function<void(std::string_view)>);
  bool PartiallyRedundant() const;
  std::string Key() const;

  static std::shared_ptr<GatewayRequest> fromIpfsPath(SlashDelimited);
};

}  // namespace ipfs::gw

inline std::ostream& operator<<(std::ostream& s, ipfs::gw::Type t) {
  return s << name(t);
}

#endif  // IPFS_TRUSTLESS_REQUEST_H_
