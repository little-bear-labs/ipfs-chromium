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

#elif __has_include(<boost/beast/core/span.hpp>)

// Prior to Boost 1.78, span did not exist in core yet
#include <boost/beast/core/span.hpp>
#include <vector>
namespace ipfs {
template <class Value>
class span : public boost::beast::span<Value> {
 public:
  span(Value* d, std::size_t n) : boost::beast::span<Value>{d, n} {}

  template <class V2>
  span(std::vector<V2> const& v)
      : boost::beast::span<Value>{v.data(), v.size()} {}

  span subspan(std::size_t off) const {
    return span{this->data() + off, this->size() - off};
  }
  Value& operator[](std::size_t i) { return this->data()[i]; }
};
}  // namespace ipfs

#else

#error \
    "No good implementation of span available. Implement one, move to a newer C++, or provide Boost or Abseil."

#endif

#endif  // IPFS_SPAN_H_
