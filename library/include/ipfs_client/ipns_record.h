#ifndef IPFS_IPNS_RECORD_H_
#define IPFS_IPNS_RECORD_H_

#include <vocab/byte_view.h>

#include "components/ipfs/keys.pb.h"

#include <functional>
#include <optional>

namespace libp2p::peer {
class PeerId;
}

namespace ipfs {

struct IpnsCborEntry {
  std::string value;
  std::string validity;
  std::uint64_t validityType;
  std::uint64_t sequence;
  std::uint64_t ttl;
};

using CborDeserializer = IpnsCborEntry(ByteView);

using CryptoSignatureVerifier = bool(ipns::KeyType,
                                     ByteView,
                                     ByteView,
                                     ByteView);

std::optional<IpnsCborEntry> ValidateIpnsRecord(
    ByteView top_level_bytes,
    libp2p::peer::PeerId const& name,
    CryptoSignatureVerifier,
    CborDeserializer);

struct ValidatedIpns {
  std::string value;
  std::time_t use_until;
  std::time_t cache_until;
  std::uint64_t sequence;

  ValidatedIpns(IpnsCborEntry const&);
  ValidatedIpns();
  std::string Serialize() const;
  static ValidatedIpns Deserialize(std::string);
};

}  // namespace ipfs

#endif  // IPFS_IPNS_RECORD_H_
