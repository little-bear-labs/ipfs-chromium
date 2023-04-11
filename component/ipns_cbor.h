#ifndef IPFS_IPNS_CBOR_H_
#define IPFS_IPNS_CBOR_H_

#include <ipfs_client/ipns_record.h>

namespace ipfs {
IpnsCborEntry ParseCborIpns(ByteView);
}

#endif  // IPFS_IPNS_CBOR_H_
