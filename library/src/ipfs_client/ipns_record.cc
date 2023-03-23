#include <ipfs_client/ipns_record.h>

#include "log_macros.h"

#if __has_include("components/ipfs/ipns_record.pb.h")
#include "components/ipfs/ipns_record.pb.h"
#else
#include "ipns_record.pb.h"
#endif

#include <libp2p/peer/peer_id.hpp>

std::string ipfs::ValidateIpnsRecord(ByteView top_level_bytes,
                                     libp2p::peer::PeerId const& name) {
  // https://github.com/ipfs/specs/blob/main/ipns/IPNS.md#record-verification

  // Before parsing the protobuf, confirm that the serialized IpnsEntry bytes
  // sum to less than or equal to the size limit.
  if (top_level_bytes.size() > 10240UL) {
    LOG(ERROR) << "IPNS record too large: " << top_level_bytes.size();
    return {};
  }

  ipfs::ipns::IpnsEntry entry;
  if (!entry.ParseFromArray(top_level_bytes.data(), top_level_bytes.size())) {
    LOG(ERROR) << "Failed to parse top-level bytes as a protobuf";
    return {};
  }

  // Confirm IpnsEntry.signatureV2 and IpnsEntry.data are present and are not
  // empty
  if (!entry.has_signaturev2()) {
    LOG(ERROR) << "IPNS record contains no .signatureV2!";
    return {};
  }
  if (!entry.has_data() || entry.data().empty()) {
    LOG(ERROR) << "IPNS record has no .data";
    return {};
  }

  // The only supported value is 0, which indicates the validity field contains
  // the expiration date after which the IPNS record becomes invalid.
  DCHECK_EQ(entry.validitytype(), 0);

  if (entry.has_pubkey()) {
    // TODO hash key & match against name
    // TODO verify signature with entry.pubkey()
  } else if (name.toMultihash().getType() ==
             libp2p::multi::HashType::identity) {
    // TODO get key from name & verify signature
  } else {
    LOG(ERROR) << "IPNS record contains no public key, and the IPNS name "
               << name.toMultihash().toHex()
               << " is a true hash, not identity. Validation impossible.";
    return {};
  }

  return entry.value();
}
