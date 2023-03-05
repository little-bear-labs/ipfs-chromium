#ifndef IPFS_BYTE_H_
#define IPFS_BYTE_H_

#include <cstddef>
#include <cstdint>

#include <iomanip>
#include <iostream>
#include <version>

#ifdef __cpp_lib_byte

namespace ipfs {
using Byte = std::byte;
}  // namespace ipfs

#else
namespace ipfs {
enum class Byte : std::uint_least8_t {};
}  // namespace ipfs
#endif

namespace {
[[maybe_unused]] std::ostream& operator<<(std::ostream& str, ipfs::Byte b) {
  return str << std::hex << static_cast<unsigned>(b);
}
}  // namespace

namespace {
// libc++ provides this, but for some reason libstdc++ does not
[[maybe_unused]] std::uint8_t to_integer(ipfs::Byte b) {
  return static_cast<std::uint8_t>(b);
}
}  // namespace

#endif  // IPFS_BYTE_H_
