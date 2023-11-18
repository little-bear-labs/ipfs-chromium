#include <libp2p/multi/multibase_codec/codecs/base36.hpp>

#include <vocab/i128.h>

#include <cmath>

#include <algorithm>

namespace det = libp2p::multi::detail;

namespace {
constexpr double kLengthRatio = 0.646240625;  // log(36)/log(256)

std::int_least16_t digit_value(char digit) {
  if (digit < '0') {
    return -1;
  } else if (digit <= '9') {
    return digit - '0';
  } else if (digit < 'A') {
    return -2;
  } else if (digit <= 'Z') {
    return (digit - 'A') + 10;
  } else if (digit < 'a') {
    return -3;
  } else if (digit <= 'z') {
    return (digit - 'a') + 10;
  } else {
    return -4;
  }
}
int operator*(int a, ipfs::Byte b) {
  return a * static_cast<std::uint_least8_t>(b);
}
}  // namespace

std::string det::encodeBase36Lower(ipfs::ByteView) {
  std::abort();
}

auto det::decodeBase36(std::string_view str_b36)
    -> ipfs::expected<common::ByteArray, BaseError> {
  common::ByteArray out;
  out.resize(std::ceil(static_cast<double>(str_b36.size()) * kLengthRatio),
             ipfs::Byte{});
  for (auto digit : str_b36) {  // chunk) {
    int val = digit_value(digit);
    if (val < 0) {
      return ipfs::unexpected<BaseError>{BaseError::INVALID_BASE36_INPUT};
    }
    auto mod_byte = [&val](auto& b) {
      val += 36 * b;
      b = static_cast<ipfs::Byte>(val & 0xFF);
      val >>= 8;
    };
    std::for_each(out.rbegin(), out.rend(), mod_byte);
  }
  return out;
}
