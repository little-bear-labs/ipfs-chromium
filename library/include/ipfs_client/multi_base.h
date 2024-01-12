#ifndef IPFS_MB_PREFIXES_H_
#define IPFS_MB_PREFIXES_H_

#include <vocab/byte_view.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ipfs::mb {

// https://github.com/multiformats/multibase/blob/master/multibase.csv
enum class Code : char {
  IDENTITY = '\0',
  UNSUPPORTED = '1',
  BASE16_LOWER = 'f',
  BASE16_UPPER = 'F',
  BASE32_LOWER = 'b',
  BASE32_UPPER = 'B',
  BASE36_LOWER = 'k',
  BASE36_UPPER = 'K',
  BASE58_BTC = 'z',
  BASE64 = 'm'
};
Code CodeFromPrefix(char c);
std::string_view GetName(Code);

using Decoder = std::vector<Byte> (*)(std::string_view);
using Encoder = std::string (*)(ByteView);
struct Codec {
  Decoder const decode;
  Encoder const encode;
  std::string_view const name;
  static Codec const* Get(Code);
};

std::string encode(Code, ByteView);
std::optional<std::vector<Byte>> decode(std::string_view mb_str);
}  // namespace ipfs::mb

#endif  // IPFS_MB_PREFIXES_H_
