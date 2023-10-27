#ifndef IPFS_TRUSTLESS_REQUEST_H_
#define IPFS_TRUSTLESS_REQUEST_H_

#include <libp2p/multi/content_identifier.hpp>

#include <vocab/slash_delimited.h>

#include <memory>
#include <optional>
#include <string>

namespace ipfs {
class IpfsRequest;
class Orchestrator;
}  // namespace ipfs

namespace ipfs::gw {
enum class Type { Block, Car, Ipns, DnsLink, Providers, Identity };

struct GatewayRequest {
  Type type;
  std::string main_param;  ///< CID, IPNS name, hostname
  std::string path;        ///< For CAR requests
  std::shared_ptr<IpfsRequest> dependent;
  std::shared_ptr<Orchestrator> orchestrator;
  std::optional<libp2p::multi::ContentIdentifier> cid;

  std::string url_suffix() const;
  std::string_view accept() const;
  std::string_view identity_data() const;
  static std::shared_ptr<GatewayRequest> fromIpfsPath(SlashDelimited);
};

}  // namespace ipfs::gw

#endif  // IPFS_TRUSTLESS_REQUEST_H_
