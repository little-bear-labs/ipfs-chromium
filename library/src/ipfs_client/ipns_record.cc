#include <ipfs_client/ipns_record.h>

#include "log_macros.h"

#if __has_include("components/ipfs/ipns_record.pb.h")
#include "components/ipfs/ipns_record.pb.h"
#else
#include "ipns_record.pb.h"
#endif

// #include <libp2p/crypto/crypto_provider/crypto_provider_impl.hpp>
#include <libp2p/crypto/hasher.hpp>
#include <libp2p/peer/peer_id.hpp>

namespace {
bool matches(libp2p::multi::Multihash const& hash,
             ipfs::ByteView pubkey_bytes) {
  auto hasher = libp2p::crypto::CreateHasher(hash.getType());
  std::basic_string<ipfs::Byte> result(hasher->digestSize(), ipfs::Byte{});
  if (hasher->write(pubkey_bytes).value()) {
    if (!hasher->digestOut({result.data(), result.size()}).has_value()) {
      LOG(ERROR) << "Error getting digest.";
    }
  } else {
    LOG(ERROR) << "Attempt to hash bytes returned false";
  }
  return std::equal(result.begin(), result.end(), hash.getHash().begin(),
                    hash.getHash().end());
}
}  // namespace
std::string ipfs::ValidateIpnsRecord(ByteView top_level_bytes,
                                     libp2p::peer::PeerId const& name,
                                     CryptoSignatureVerifier verify) {
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

  ipfs::ByteView public_key;
  if (entry.has_pubkey()) {
    public_key = ipfs::ByteView{
        reinterpret_cast<ipfs::Byte const*>(entry.pubkey().data()),
        entry.pubkey().size()};
    if (!matches(name.toMultihash(), public_key)) {
      LOG(ERROR) << "Given IPNS record contains a pubkey that does not match "
                    "the hash from the IPNS name that fetched it!";
      return {};
    }
  } else if (name.toMultihash().getType() ==
             libp2p::multi::HashType::identity) {
    public_key = name.toMultihash().getHash();
  } else {
    LOG(ERROR) << "IPNS record contains no public key, and the IPNS name "
               << name.toMultihash().toHex()
               << " is a true hash, not identity. Validation impossible.";
    return {};
  }
  ipfs::ipns::PublicKey pk;
  auto* pkbp = reinterpret_cast<char const*>(public_key.data());
  if (!pk.ParseFromArray(pkbp, public_key.size())) {
    LOG(ERROR) << "Failed to parse public key bytes";
    return {};
  }
  LOG(INFO) << "Record contains a public key of type " << pk.type()
            << " and points to " << entry.value();
  auto& signature_str = entry.signaturev2();
  ByteView signature{reinterpret_cast<ipfs::Byte const*>(signature_str.data()),
                     signature_str.size()};
  // https://specs.ipfs.tech/ipns/ipns-record/#record-verification
  //  Create bytes for signature verification by concatenating ipns-signature:
  //  prefix (bytes in hex: 69706e732d7369676e61747572653a) with raw CBOR bytes
  //  from IpnsEntry.data
  auto bytes_str = entry.data();
  bytes_str.insert(
      0, "\x69\x70\x6e\x73\x2d\x73\x69\x67\x6e\x61\x74\x75\x72\x65\x3a");
  ByteView bytes{reinterpret_cast<ipfs::Byte const*>(bytes_str.data()),
                 bytes_str.size()};
  ByteView key_bytes{reinterpret_cast<ipfs::Byte const*>(pk.data().data()),
                     pk.data().size()};
  if (verify(pk.type(), signature, bytes, key_bytes)) {
    LOG(INFO) << "Verification passed.";
    return entry.value();
  } else {
    LOG(ERROR) << "Verification failed!!";
    return {};
  }
}
