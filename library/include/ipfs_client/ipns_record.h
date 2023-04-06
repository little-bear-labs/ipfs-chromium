#ifndef IPFS_IPNS_RECORD_H_
#define IPFS_IPNS_RECORD_H_

#include <vocab/byte_view.h>

namespace libp2p::peer {
class PeerId;
}

namespace ipfs {
std::string ValidateIpnsRecord(ByteView top_level_bytes,
                               libp2p::peer::PeerId const& name);

}  // namespace ipfs

#endif  // IPFS_IPNS_RECORD_H_
