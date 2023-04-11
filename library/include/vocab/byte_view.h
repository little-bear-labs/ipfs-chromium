#ifndef CHROMIUM_IPFS_BYTE_VIEW_H
#define CHROMIUM_IPFS_BYTE_VIEW_H

#include "byte.h"
#include "span.h"

#include <cstdint>

namespace ipfs {
using ByteView = span<ipfs::Byte const>;

inline span<std::uint8_t const> as_octets(ByteView bytes) {
  return {reinterpret_cast<std::uint8_t const*>(bytes.data()), bytes.size()};
}
}  // namespace ipfs

#endif  // CHROMIUM_IPFS_BYTE_VIEW_H
