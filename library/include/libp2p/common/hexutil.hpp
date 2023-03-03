/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_HEXUTIL_HPP
#define LIBP2P_HEXUTIL_HPP

#include "vocab/byte_view.h"
#include "vocab/expected.h"

#include <string_view>
#include <vector>

namespace libp2p::common {

/**
 * @brief error codes for exceptions that may occur during unhexing
 */
enum class UnhexError { NOT_ENOUGH_INPUT = 1, NON_HEX_INPUT, UNKNOWN };

/**
 * @brief Converts an integer to an uppercase hex representation
 */
std::string int_to_hex(uint64_t n, size_t fixed_width = 2) noexcept;

/**
 * @brief Converts bytes to uppercase hex representation
 * @param array bytes
 * @param len length of bytes
 * @return hexstring
 */
std::string hex_upper(ipfs::ByteView bytes) noexcept;

/**
 * @brief Converts bytes to hex representation
 * @param array bytes
 * @param len length of bytes
 * @return hexstring
 */
std::string hex_lower(ipfs::ByteView bytes) noexcept;

/**
 * @brief Converts hex representation to bytes
 * @param array individual chars
 * @param len length of chars
 * @return result containing array of bytes if input string is hex encoded and
 * has even length
 *
 * @note reads both uppercase and lowercase hexstrings
 *
 * @see
 * https://www.boost.org/doc/libs/1_51_0/libs/algorithm/doc/html/the_boost_algorithm_library/Misc/hex.html
 */
ipfs::expected<std::vector<ipfs::Byte>, UnhexError> unhex(std::string_view hex);

/**
 * Creates unsigned bytes span out of string, debug purpose helper
 * @param str String
 * @return Span
 */
inline ipfs::ByteView sv2span(const std::string_view& str) {
  return ipfs::ByteView((ipfs::Byte*)str.data(),  // NOLINT
                        str.size());              // NOLINT
}

/**
 * sv2span() identity overload, for uniformity
 * @param s Span
 * @return s
 */
inline ipfs::ByteView sv2span(const ipfs::ByteView& s) {
  return s;
}

/**
 * Makes printable string out of bytes, for diagnostic purposes
 * @tparam Bytes Bytes or char sequence
 * @param str Input
 * @return String
 */
template <class Bytes>
inline std::string dumpBin(const Bytes& str) {
  std::string ret;
  ret.reserve(str.size() + 2);
  bool non_printable_detected = false;
  for (auto c : str) {
    if (std::isprint(c) != 0) {
      ret.push_back((char)c);  // NOLINT
    } else {
      ret.push_back('?');
      non_printable_detected = true;
    }
  }
  if (non_printable_detected) {
    ret.reserve(ret.size() * 3);
    ret += " (";
    ret += hex_lower(sv2span(str));
    ret += ')';
  }
  return ret;
}

}  // namespace libp2p::common

#endif  // LIBP2P_HEXUTIL_HPP
