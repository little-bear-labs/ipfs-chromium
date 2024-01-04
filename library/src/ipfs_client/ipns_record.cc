#include <ipfs_client/ipns_record.h>

#include <ipfs_client/cid.h>
#include <ipfs_client/context_api.h>
#include <ipfs_client/dag_cbor_value.h>

#include "log_macros.h"

#include <ctime>
#include <sstream>

#if __has_include(<third_party/ipfs_client/ipns_record.pb.h>)
#include <third_party/ipfs_client/ipns_record.pb.h>
#else
#include "ipfs_client/ipns_record.pb.h"
#endif

namespace {
bool matches(ipfs::MultiHash const& hash,
             ipfs::ByteView pubkey_bytes,
             ipfs::ContextApi& api) {
  auto result = api.Hash(hash.type(), pubkey_bytes);
  if (!result.has_value()) {
    return false;
  }
  return std::equal(result->begin(), result->end(), hash.digest().begin(),
                    hash.digest().end());
}
}  // namespace

namespace {
void assign(std::string& out,
            ipfs::DagCborValue& top,
            std::string_view key) {
  auto p = top.at(key);
  if (!p) {
    out.assign("Key '").append(key).append("' not present in IPNS CBOR!");
  } else {
    // YEP! as_bytes() . There are only 2 string values here, they are logically
    // text, but they are defined in the spec to be bytes.
    auto o = p->as_bytes();
    if (o.has_value()) {
      auto chars = reinterpret_cast<char const*>(o.value().data());
      out.assign(chars, o.value().size());
    } else {
      out.assign("Key '").append(key).append(
          "' was not a string in IPNS CBOR!");
    }
  }
}
void assign(std::uint64_t& out,
            ipfs::DagCborValue& top,
            std::string_view key) {
  auto p = top.at(key);
  if (!p) {
    LOG(ERROR) << "Key '" << key << "' is not present in IPNS CBOR!";
    out = std::numeric_limits<std::uint64_t>::max();
  } else {
    auto o = p->as_unsigned();
    if (o.has_value()) {
      out = o.value();
    } else {
      LOG(ERROR) << "Key '" << key
                 << "' is not an unsigned integer in IPNS CBOR!";
      out = std::numeric_limits<std::uint64_t>::max();
    }
  }
}
}  // namespace

auto ipfs::ValidateIpnsRecord(ipfs::ByteView top_level_bytes,
                              Cid const& name,
                              ContextApi& api) -> std::optional<IpnsCborEntry> {
  DCHECK_EQ(name.codec(), MultiCodec::LIBP2P_KEY);
  if (name.codec() != MultiCodec::LIBP2P_KEY) {
    return {};
  }
  // https://github.com/ipfs/specs/blob/main/ipns/IPNS.md#record-verification

  // Before parsing the protobuf, confirm that the serialized IpnsEntry bytes
  // sum to less than or equal to the size limit.
  if (top_level_bytes.size() > MAX_IPNS_PB_SERIALIZED_SIZE) {
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

  auto parsed =
      api.ParseCbor({reinterpret_cast<Byte const*>(entry.data().data()),
                     entry.data().size()});
  if (!parsed) {
    LOG(ERROR) << "CBOR parsing failed.";
    return {};
  }
  IpnsCborEntry result;
  assign(result.value, *parsed, "Value");
  if (entry.has_value() && result.value != entry.value()) {
    LOG(ERROR) << "Mismatch on Value field in IPNS record... CBOR(v2): '"
               << result.value << "' but PB(v1): '" << entry.value()
               << "' : " << parsed->html();
    return {};
  }
  ipfs::ByteView public_key;
  if (entry.has_pubkey()) {
    public_key = ipfs::ByteView{
        reinterpret_cast<ipfs::Byte const*>(entry.pubkey().data()),
        entry.pubkey().size()};
    if (!matches(name.multi_hash(), public_key, api)) {
      LOG(ERROR) << "Given IPNS record contains a pubkey that does not match "
                    "the hash from the IPNS name that fetched it!";
      return {};
    }
  } else if (name.hash_type() == HashType::IDENTITY) {
    public_key = name.hash();
  } else {
    LOG(ERROR) << "IPNS record contains no public key, and the IPNS name "
               << name.to_string()
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
  //  Create bytes for signature verification by concatenating
  //  ipto_hex(ns-signature://  prefix (bytes in hex:
  //  69706e732d7369676e61747572653a) with raw CBOR bytes from IpnsEntry.data
  auto bytes_str = entry.data();
  bytes_str.insert(
      0, "\x69\x70\x6e\x73\x2d\x73\x69\x67\x6e\x61\x74\x75\x72\x65\x3a");
  ByteView bytes{reinterpret_cast<ipfs::Byte const*>(bytes_str.data()),
                 bytes_str.size()};
  ByteView key_bytes{reinterpret_cast<ipfs::Byte const*>(pk.data().data()),
                     pk.data().size()};
  if (!api.VerifyKeySignature(static_cast<SigningKeyType>(pk.type()), signature,
                              bytes, key_bytes)) {
    LOG(ERROR) << "Verification failed!!";
    return {};
  }
  // TODO check expiration date
  if (entry.has_value() && entry.value() != result.value) {
    LOG(ERROR) << "IPNS " << name.to_string() << " has different values for V1("
               << entry.value() << ") and V2(" << result.value << ')';
    return {};
  }
  assign(result.validity, *parsed, "Validity");
  if (entry.has_validity() && entry.validity() != result.validity) {
    LOG(ERROR) << "IPNS " << name.to_string()
               << " has different validity for V1(" << entry.validity()
               << ") and V2(" << result.validity << ')';
    return {};
  }
  assign(result.validityType, *parsed, "ValidityType");
  if (entry.has_validitytype() &&
      entry.validitytype() != static_cast<int>(result.validityType)) {
    LOG(ERROR) << "IPNS " << name.to_string()
               << " has different validity types for V1("
               << entry.validitytype() << ") and V2(" << result.validityType
               << ')';
    return {};
  }
  assign(result.sequence, *parsed, "Sequence");
  if (entry.has_sequence() && entry.sequence() != result.sequence) {
    LOG(ERROR) << "IPNS " << name.to_string()
               << " has different validity types for V1(" << entry.sequence()
               << ") and V2(" << result.sequence << ')';
    return {};
  }
  assign(result.ttl, *parsed, "TTL");
  if (entry.has_ttl() && entry.ttl() != result.ttl) {
    LOG(ERROR) << "IPNS " << name.to_string()
               << " has different validity types for V1(" << entry.ttl()
               << ") and V2(" << result.ttl << ')';
    return {};
  }
  VLOG(1) << "IPNS record verification passes for " << name.to_string()
          << " sequence: " << result.sequence << " points at "
            << result.value;
  return result;
}

ipfs::ValidatedIpns::ValidatedIpns() = default;
ipfs::ValidatedIpns::ValidatedIpns(ValidatedIpns&&) = default;
ipfs::ValidatedIpns::ValidatedIpns(ValidatedIpns const&) = default;
auto ipfs::ValidatedIpns::operator=(ValidatedIpns const&)
    -> ValidatedIpns& = default;
ipfs::ValidatedIpns::ValidatedIpns(IpnsCborEntry const& e)
    : value{e.value}, sequence{e.sequence} {
  std::istringstream ss{e.validity};
  std::tm t = {};
  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
  long ttl = (e.ttl / 1'000'000'000UL) + 1;
#ifdef _MSC_VER
  use_until = _mkgmtime(&t);
#else
  use_until = timegm(&t);
#endif
  cache_until = std::time(nullptr) + ttl;
}

std::string ipfs::ValidatedIpns::Serialize() const {
  DCHECK_EQ(value.find(' '), std::string::npos);
  DCHECK_EQ(gateway_source.find(' '), std::string::npos);
  std::ostringstream ss;
  ss << std::hex << sequence << ' ' << use_until << ' ' << cache_until << ' '
     << fetch_time << ' ' << resolution_ms << ' ' << value << ' '
     << gateway_source;
  return ss.str();
}
auto ipfs::ValidatedIpns::Deserialize(std::string s) -> ValidatedIpns {
  std::istringstream ss(s);
  ValidatedIpns e;
  ss >> std::hex >> e.sequence >> e.use_until >> e.cache_until >>
      e.fetch_time >> e.resolution_ms >> e.value >> e.gateway_source;
  return e;
}
