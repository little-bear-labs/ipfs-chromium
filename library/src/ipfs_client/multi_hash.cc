#include <ipfs_client/multi_hash.h>

#include <iterator>
#include <libp2p/multi/uvarint.hpp>
#include "vocab/byte_view.h"
#include <string_view>

using Self = ipfs::MultiHash;
using VarInt = libp2p::multi::UVarint;

Self::MultiHash(ipfs::HashType t, ipfs::ByteView digest)
    : type_{t}, hash_(digest.begin(), digest.end()) {}

Self::MultiHash(ipfs::ByteView bytes) {
  ReadPrefix(bytes);
}
auto Self::ReadPrefix(ipfs::ByteView& bytes) -> bool {
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
auto Self::valid() const -> bool {
  return type_ != HashType::INVALID && !hash_.empty();
}
namespace {
constexpr std::string_view InvalidHashTypeName;
}
auto ipfs::GetName(HashType t) -> std::string_view {
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