#ifndef IPFS_TRUSTLESS_REQUEST_H_
#define IPFS_TRUSTLESS_REQUEST_H_

#include <ipfs_client/context_api.h>

#include <libp2p/multi/content_identifier.hpp>

#include <vocab/slash_delimited.h>

#include <iosfwd>
#include <memory>
#include <optional>
#include <string>

namespace ipfs {
class IpfsRequest;
class Orchestrator;
}  // namespace ipfs

namespace ipfs::gw {
class Requestor;

enum class Type { Block, Car, Ipns, DnsLink, Providers, Identity, Zombie };

constexpr std::size_t BLOCK_RESPONSE_BUFFER_SIZE = 2 * 1024 * 1024;

struct GatewayRequest {
  Type type;
  std::string main_param;  ///< CID, IPNS name, hostname
  std::string path;        ///< For CAR requests
  std::shared_ptr<IpfsRequest> dependent;
  std::shared_ptr<Orchestrator> orchestrator;
  std::shared_ptr<Requestor> retry_at;
  std::optional<libp2p::multi::ContentIdentifier> cid;
  short parallel = 0;
  std::string affinity;

  std::string url_suffix() const;
  std::string_view accept() const;
  std::string_view identity_data() const;
  short timeout_seconds() const;
  bool is_http() const;
  std::optional<std::size_t> max_response_size() const;
  std::optional<HttpRequestDescription> describe_http() const;
  std::string debug_string() const;

  static std::shared_ptr<GatewayRequest> fromIpfsPath(SlashDelimited);
};

}  // namespace ipfs::gw

std::ostream& operator<<(std::ostream&, ipfs::gw::Type);

#endif  // IPFS_TRUSTLESS_REQUEST_H_
