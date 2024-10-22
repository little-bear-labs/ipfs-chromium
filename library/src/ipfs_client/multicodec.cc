#include <ipfs_client/multicodec.h>
#include <string_view>

using Cdc = ipfs::MultiCodec;

namespace {
constexpr std::string_view InvalidMulticodecLabel{"invalid-multicodec"};
}

auto ipfs::GetName(Cdc codec) -> std::string_view {
  switch (codec) {
    case Cdc::INVALID:
      return InvalidMulticodecLabel;
    case Cdc::IDENTITY:
      return "identity";
    case Cdc::RAW:
      return "raw";
    case Cdc::DAG_PB:
      return "dag-pb";
    case Cdc::DAG_CBOR:
      return "dag-cbor";
    case Cdc::LIBP2P_KEY:
      return "libp2p-key";
    case Cdc::DAG_JSON:
      return "dag-json";
  }
  return InvalidMulticodecLabel;
}
auto ipfs::Validate(Cdc c) -> Cdc {
  if (GetName(c) == InvalidMulticodecLabel) {
    return Cdc::INVALID;
  }
  return c;
}
