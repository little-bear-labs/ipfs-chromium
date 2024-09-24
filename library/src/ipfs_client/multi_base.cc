#include "ipfs_client/multi_base.h"

#include "bases/b16_upper.h"
#include "bases/b32.h"

#include <multibase/basic_algorithm.h>

#include "log_macros.h"

using namespace std::literals;

namespace imb = ipfs::mb;
namespace {
constexpr std::string_view UnsupportedMultibase = "unsupported-multibase";

template <class Target>
std::string encode_adapt(ipfs::ByteView bytes) {
  auto p = reinterpret_cast<char const*>(bytes.data());
  typename Target::encoder target;
  return target.process({p, bytes.size()});
}
enum class EncodedCase { lower, UPPER, Sensitive };
template <class Target, EncodedCase const ec>
std::vector<ipfs::Byte> decode_adapt(std::string_view encoded_sv) {
  typename Target::decoder target;
  std::string encoded_s{encoded_sv};
  switch (ec) {
    case EncodedCase::lower:
      for (auto& c : encoded_s) {
        if (c >= 'A' && c <= 'Z') {
          c = c - 'A' + 'a';
        }
      }
      break;
    case EncodedCase::UPPER:
      for (auto& c : encoded_s) {
        if (c >= 'a' && c <= 'z') {
          c = c - 'a' + 'A';
        }
      }
      break;
    case EncodedCase::Sensitive:
      break;
  }
  auto s = target.process(encoded_s);
  auto b = reinterpret_cast<ipfs::Byte const*>(s.data());
  auto e = b + s.size();
  return std::vector<ipfs::Byte>(b, e);
}
template <class BasicAlgoSpecl, EncodedCase const ec>
constexpr imb::Codec adapt(std::string_view name) {
  return imb::Codec{&decode_adapt<BasicAlgoSpecl, ec>,
                    &encode_adapt<BasicAlgoSpecl>, name};
}
}  // namespace

auto imb::Codec::Get(Code c) -> Codec const* {
  switch (c) {
    case Code::IDENTITY:
    case Code::UNSUPPORTED:
    case Code::BASE64:
      return nullptr;
    case Code::BASE16_LOWER: {
      static auto b16 =
          adapt<multibase::base_16, EncodedCase::lower>("base16"sv);
      return &b16;
    }
    case Code::BASE16_UPPER: {
      static auto b16u =
          adapt<base_16_upper, EncodedCase::UPPER>("base16upper"sv);
      return &b16u;
    }
    case Code::BASE32_LOWER: {
      static auto b32 = adapt<base_32, EncodedCase::lower>("base32"sv);
      return &b32;
    }
    case Code::BASE32_UPPER: {
      static auto b32u =
          adapt<base_32_upper, EncodedCase::UPPER>("base32upper"sv);
      return &b32u;
    }
    case Code::BASE36_LOWER: {
      static auto b36 =
          adapt<multibase::base_36_btc, EncodedCase::lower>("base36"sv);
      return &b36;
    }
    case Code::BASE36_UPPER: {
      static auto b36u =
          adapt<multibase::base_36_btc, EncodedCase::UPPER>("base36upper"sv);
      return &b36u;
    }
    case Code::BASE58_BTC: {
      static auto b58 =
          adapt<multibase::base_58_btc, EncodedCase::Sensitive>("base58btc"sv);
      return &b58;
    }
  }
  return nullptr;
}
std::string_view imb::GetName(Code c) {
  if (auto codec = Codec::Get(c)) {
    return codec->name;
  }
  return UnsupportedMultibase;
}
auto imb::CodeFromPrefix(char ch) -> Code {
  auto c = static_cast<Code>(ch);
  return Codec::Get(c) ? Code::UNSUPPORTED : c;
}
auto imb::decode(std::string_view mb_str) -> std::optional<std::vector<Byte>> {
  if (mb_str.empty()) {
    return std::nullopt;
  }
  if (auto* codec = Codec::Get(static_cast<Code>(mb_str[0]))) {
    return codec->decode(mb_str.substr(1));
  } else {
    return std::nullopt;
  }
}
std::string imb::encode(Code c, ByteView bs) {
  if (auto codec = Codec::Get(c)) {
    auto rv = codec->encode(bs);
    if (rv.size() >= bs.size()) {
      rv.insert(0UL, 1UL, static_cast<char>(c));
      return rv;
    } else {
      LOG(ERROR) << "Error encoding into base " << codec->name;
    }
  } else {
    LOG(ERROR) << "Can't encode to multibase " << static_cast<char>(c)
               << " because I can't find a codec??";
  }
  return {};
}
