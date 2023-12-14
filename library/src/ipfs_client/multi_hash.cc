#include <ipfs_client/multi_hash.h>

#include <libp2p/multi/uvarint.hpp>

using Self = ipfs::MultiHash;
using VarInt = libp2p::multi::UVarint;

Self::MultiHash(ipfs::HashType t, ipfs::ByteView digest)
    : type_{t}, hash_(digest.begin(), digest.end()) {}

Self::MultiHash(ipfs::ByteView bytes) {
  ReadPrefix(bytes);
}
bool Self::ReadPrefix(ipfs::ByteView& bytes) {
  auto i = VarInt::create(bytes);
  if (!i) {
    return false;
  }
  bytes = bytes.subspan(i->size());
  auto type = Validate(static_cast<HashType>(i->toUInt64()));
  i = VarInt::create(bytes);
  if (!i) {
    return false;
  }
  auto length = i->toUInt64();
  if (length > bytes.size()) {
    return false;
  }
  bytes = bytes.subspan(i->size());
  hash_.assign(bytes.begin(), std::next(bytes.begin(), length));
  bytes = bytes.subspan(length);
  type_ = type;
  return true;
}
bool Self::valid() const {
  return type_ != HashType::INVALID && hash_.size() > 0UL;
}
namespace {
constexpr std::string_view InvalidHashTypeName;
}
std::string_view ipfs::GetName(HashType t) {
  switch (t) {
    case HashType::INVALID:
      return InvalidHashTypeName;
    case HashType::IDENTITY:
      return "identity";
    case HashType::SHA2_256:
      return "sha2-256";
  }
  // Don't use default: -> let it fall through. We want compiler warnings about
  // unhandled cases.
  return InvalidHashTypeName;
}
auto ipfs::Validate(HashType t) -> HashType {
  return GetName(t) == InvalidHashTypeName ? HashType::INVALID : t;
}