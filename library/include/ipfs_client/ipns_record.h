#ifndef IPFS_IPNS_RECORD_H_
#define IPFS_IPNS_RECORD_H_

#include <vocab/byte_view.h>

#if __has_include("components/ipfs/keys.pb.h")
#include "components/ipfs/keys.pb.h"
#else
#include "keys.pb.h"
#endif

#include <functional>

namespace libp2p::peer {
class PeerId;
}

namespace ipfs {

using CryptoSignatureVerifier = std::function<
    bool(ipfs::ipns::KeyType, ipfs::ByteView, ipfs::ByteView, ipfs::ByteView)>;

std::string ValidateIpnsRecord(ByteView top_level_bytes,
                               libp2p::peer::PeerId const& name,
                               CryptoSignatureVerifier);

}  // namespace ipfs

#endif  // IPFS_IPNS_RECORD_H_
