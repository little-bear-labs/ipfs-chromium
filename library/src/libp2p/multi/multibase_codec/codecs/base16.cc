#include <libp2p/multi/multibase_codec/codecs/base16.h>

namespace b16 = ipfs::base16;

namespace {
std::uint8_t to_i(char c);
template <char const a>
char to_c(std::uint8_t n) {
  if (n < 10) {
    return n + '0';
  } else {
    return n - 10 + a;
  }
}
template <char const a>
std::string encode(ipfs::ByteView bytes) {
  std::string result;
  result.reserve(bytes.size() * 2);
  for (auto b : bytes) {
    auto i = to_integer(b);
    result.push_back(to_c<a>(i >> 4));
    result.push_back(to_c<a>(i & 0xF));
  }
  return result;
}
}  // namespace

std::string b16::encodeLower(ByteView bytes) {
  return encode<'a'>(bytes);
}
std::string b16::encodeUpper(ByteView bytes) {
  return encode<'A'>(bytes);
}
auto b16::decode(std::string_view s) -> Decoded {
  ByteArray result(s.size() / 2, ipfs::Byte{});
  for (auto i = 0U; i + 1U < s.size(); i += 2U) {
    auto a = to_i(s[i]);
    auto b = to_i(s[i + 1]);
    if (a > 0xF || b > 0xF) {
      return ipfs::unexpected<BaseError>{BaseError::INVALID_BASE16_INPUT};
    }
    result[i / 2] = ipfs::Byte{static_cast<std::uint8_t>((a << 4) | b)};
  }
  if (s.size() % 2) {
    auto a = to_i(s.back());
    if (a <= 0xF) {
      result.push_back(ipfs::Byte{a});
    }
  }
  return result;
}

namespace {
std::uint8_t to_i(char c) {
  switch (c) {
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case 'a':
      return 10;
    case 'b':
      return 11;
    case 'c':
      return 12;
    case 'd':
      return 13;
    case 'e':
      return 14;
    case 'f':
      return 15;
    case 'A':
      return 10;
    case 'B':
      return 11;
    case 'C':
      return 12;
    case 'D':
      return 13;
    case 'E':
      return 14;
    case 'F':
      return 15;
    default:
      return 0xFF;
  }
}
}  // namespace
