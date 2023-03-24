#include <libp2p/multi/multibase_codec/codecs/base36.hpp>

namespace det = libp2p::multi::detail;

namespace {
using namespace std::literals;
// constexpr auto kUpperAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"sv;
// constexpr auto kLowerAlphabet = "abcdefghijklmnopqrstuvwxyz234567"sv;
}  // namespace

std::string det::encodeBase36Lower(ipfs::ByteView bytes) {
  std::abort();
}

auto det::decodeBase36(std::string_view string)
    -> ipfs::expected<common::ByteArray, BaseError> {
  std::abort();
}