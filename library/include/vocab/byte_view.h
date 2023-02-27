#ifndef CHROMIUM_IPFS_BYTE_VIEW_H
#define CHROMIUM_IPFS_BYTE_VIEW_H

#include <version>

#include <cstddef>
#include <cstdint>

#ifdef __cpp_lib_byte

#include <cstddef>
namespace ipfs {
using Byte = std::byte;
}  // namespace ipfs

// libc++ provides this, but for some reason libstdc++ does not
std::uint8_t to_integer(ipfs::Byte b);

#else
namespace ipfs {
#include <cstdint>
namespace ipfs {
using Byte = std::uint_least8_t;
}
}  // namespace ipfs
#endif

#if __has_include("base/containers/span.h")

#include "base/containers/span.h"
namespace ipfs {
template <class Value>
using span = base::span<Value>;
using ByteView = span<Byte const>;
}  // namespace ipfs

#elif __has_cpp_attribute(__cpp_lib_span)

#include <span>
namespace ipfs {
template <class Value>
using span = std::span<Value>;
using ByteView = span<Byte const>;
}  // namespace ipfs

#elif __has_include(<absl/types/span.h>)

#include <absl/types/span.h>
namespace ipfs {
template <class Value>
using span = absl::Span<Value>;
using ByteView = span<Byte const>;
}  // namespace ipfs

#elif __has_include(<boost/core/span.hpp>)

#include <boost/core/span.hpp>
namespace ipfs {
template <class Value>
using span = boost::span<Value>;
using ByteView = span<Byte const>;
}  // namespace ipfs

#else

#include <string_view>

namespace ipfs {
// TODO - check that the parts of the span API being used are actually supported
// here
using ByteView = std::basic_string_view<Byte const>;
}  // namespace ipfs

#endif

#endif  // CHROMIUM_IPFS_BYTE_VIEW_H
