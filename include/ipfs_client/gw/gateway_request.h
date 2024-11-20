#ifndef IPFS_TRUSTLESS_REQUEST_H_
#define IPFS_TRUSTLESS_REQUEST_H_

#include "gateway_request_type.h"

#include <ipfs_client/ipld/block_source.h>
#include <ipfs_client/cid.h>
#include <ipfs_client/client.h>

#include <vocab/slash_delimited.h>

#include <iosfwd>
#include <memory>
#include <optional>
#include <string>
#include <unordered_set>

namespace ipfs {
class Car;
class IpfsRequest;
class Partition;
namespace ipld {
class DagNode;
}
}  // namespace ipfs

namespace ipfs::gw {
class Requestor;

std::string_view name(GatewayRequestType);

constexpr std::size_t BLOCK_RESPONSE_BUFFER_SIZE = 2UL * 1024UL * 1024UL;
constexpr std::size_t CAR_RESPONSE_BUFFER_SIZE = 5UL * 1024UL * 1024UL;

/*! Information about a request that needs to be sent to an IPFS HTTP Gateway
 * @todo this class has gotten large enough that it should probably be a base
 *    class with subclasses instead of switch statements on type
 */
class GatewayRequest : public std::enable_shared_from_this<GatewayRequest> {
 public:
  /*! Type for callbacks for when bytes are received
   */
  using BytesReceivedHook =
      std::function<void(std::string_view, ByteView, ipld::BlockSource const&)>;

 private:
  std::optional<Cid> cid_;
  std::shared_ptr<Partition> orchestrator_;
  std::vector<BytesReceivedHook> bytes_received_hooks;
  std::string main_param;  ///< CID, IPNS name, hostname

  void FleshOut(ipld::BlockSource&) const;
  void AddDnsLink(std::string_view target, bool& success, ipld::BlockSource src);
  void AddBlock(std::string_view bytes,
                bool& success,
                ipld::BlockSource src,
                std::shared_ptr<Client> const& api,
                bool* valid);
  void AddBlocks(Car& car,
                 std::shared_ptr<Client> const& api,
                 bool& success, bool* valid, ipld::BlockSource src);
  bool IpnsResponse(ByteView bytes, std::shared_ptr<Client> const& api, bool& success, bool* valid, ipld::BlockSource src);

 public:
  GatewayRequestType type = GatewayRequestType::Zombie;

  // TODO - encapsulate. Hopefully these public data members aren't directly accessed everywhere
  std::string path;        ///< For CAR requests
  std::shared_ptr<IpfsRequest> dependent;
  short parallel = 0;
  std::string affinity;
  std::unordered_set<std::string> failures;

  std::optional<Cid> const& cid() const;
  void cid(Cid c);
  std::string url_suffix() const;
  std::string_view accept() const;
  std::string_view identity_data() const;
  short timeout_seconds() const;
  bool is_http() const;
  std::optional<std::size_t> max_response_size() const;
  std::optional<HttpRequestDescription> describe_http(std::string_view) const;
  std::string debug_string() const;
  void orchestrator(std::shared_ptr<Partition> const&);
  bool cachable() const;

  /*! @return The part of the path that functions as an origin (the second component)
   */
  std::string_view root_component() const;
  void root_component(std::string_view);

  bool RespondSuccessfully(std::string_view,
                           std::shared_ptr<Client> const& api,
                           ipld::BlockSource src,
                           std::string_view roots = "",
                           bool* valid = nullptr);
  void Hook(BytesReceivedHook);
  bool PartiallyRedundant() const;
  std::string Key() const;
  bool Finished() const;

  static std::shared_ptr<GatewayRequest> fromIpfsPath(SlashDelimited ipfs_path);
};

}  // namespace ipfs::gw

inline std::ostream& operator<<(std::ostream& s,
                                ipfs::gw::GatewayRequestType t) {
  return s << name(t);
}

#endif  // IPFS_TRUSTLESS_REQUEST_H_
