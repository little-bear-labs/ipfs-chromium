#ifndef IPFS_IPNS_RECORD_H_
#define IPFS_IPNS_RECORD_H_

#include <vocab/byte_view.h>

#include "components/ipfs/keys.pb.h"

#include <functional>

namespace libp2p::peer {
class PeerId;
}

namespace ipfs {

struct IpnsCborEntry {
  std::string value;
  //  std::basic_string<Byte> validity;
  std::string validity;
  std::uint64_t validityType;
  std::uint64_t sequence;
  std::uint64_t ttl;
};

using CborDeserializer = IpnsCborEntry(ByteView);

// using CryptoSignatureVerifier = std::function<bool(ipfs::ipns::KeyType,
// ipfs::ByteView, ipfs::ByteView, ipfs::ByteView)>;
using CryptoSignatureVerifier = bool(ipns::KeyType,
                                     ByteView,
                                     ByteView,
                                     ByteView);

std::string ValidateIpnsRecord(ByteView top_level_bytes,
                               libp2p::peer::PeerId const& name,
                               CryptoSignatureVerifier,
                               CborDeserializer);

}  // namespace ipfs

#endif  // IPFS_IPNS_RECORD_H_
