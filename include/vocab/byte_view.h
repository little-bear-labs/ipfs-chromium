#ifndef CHROMIUM_IPFS_BYTE_VIEW_H
#define CHROMIUM_IPFS_BYTE_VIEW_H

#include "byte.h"
#include "span.h"

#include <cstdint>

namespace ipfs {
using ByteView = span<ipfs::Byte const>;

// ByteView is a view over arbitrary opaque byte
// Cast it to a view over 8-bit unsigned integers for inspection
inline span<std::uint8_t const> as_octets(ByteView bytes) {
  return {reinterpret_cast<std::uint8_t const*>(bytes.data()), bytes.size()};
}
inline span<std::uint8_t const> as_octets(std::string const& s) {
  return {reinterpret_cast<std::uint8_t const*>(s.data()), s.size()};
}
template <class ContiguousBytes>
inline ByteView as_bytes(ContiguousBytes const& b) {
  auto p = reinterpret_cast<ipfs::Byte const*>(b.data());
  return ByteView{p, b.size()};
}
}  // namespace ipfs

#endif  // CHROMIUM_IPFS_BYTE_VIEW_H
