#ifndef IPFS_EXPECTED_H_
#define IPFS_EXPECTED_H_

// std::expected isn't available until C++23 and we need to support C++17
// boost::outcome isn't available inside the Chromium tree
// absl::StatusOr doesn't allow templating or extending the error type, and
// translating the specific error codes into generic ones isn't great.

#if __has_include("base/types/expected.h")
#include "base/types/expected.h"
namespace ipfs {
template <class Value, class Error>
using expected = base::expected<Value, Error>;
}
#elif __has_cpp_attribute(__cpp_lib_expected)
#include <expected>
namespace ipfs {
template <class Value, class Error>
using expected = std::expected<Value, Error>;
}
#elif __has_include(<boost/outcome.hpp>)
// If the API differences between std::expected and boost::outcome::checked
// become a problem, consider wrapping as proposed in the FAQ:
// https://www.boost.org/doc/libs/master/libs/outcome/doc/html/faq.html#how-far-away-from-the-proposed-std-expected-t-e-is-outcome-s-checked-t-e
#include <boost/outcome.hpp>
namespace ipfs {
template <class Value, class Error>
using expected = boost::outcome_v2::checked<Value, Error>;
}
#else
#error Get an expected implementation
#endif

#endif  // IPFS_EXPECTED_H_
