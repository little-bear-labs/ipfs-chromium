#ifndef IPFS_SPAN_H_
#define IPFS_SPAN_H_

#if __has_include("base/containers/span.h")

#include "base/containers/span.h"
namespace ipfs {
template <class Value>
using span = base::span<Value>;
}  // namespace ipfs

#elif __has_cpp_attribute(__cpp_lib_span)

#include <span>
namespace ipfs {
template <class Value>
using span = std::span<Value>;
}  // namespace ipfs

#elif __has_include(<absl/types/span.h>)

#include <absl/types/span.h>
namespace ipfs {
template <class Value>
using span = absl::Span<Value>;
}  // namespace ipfs

#elif __has_include(<boost/core/span.hpp>)

#include <boost/core/span.hpp>
namespace ipfs {
template <class Value>
using span = boost::span<Value>;
}  // namespace ipfs

#else

#include <string_view>

namespace ipfs {
// TODO - check that the parts of the span API being used are actually supported
template <class Value>
using span = std::basic_string_view<Value>;
}  // namespace ipfs

#endif

#endif  // IPFS_SPAN_H_
