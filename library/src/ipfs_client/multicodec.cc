#include <ipfs_client/multicodec.h>

using Cdc = ipfs::MultiCodec;

namespace {
constexpr std::string_view InvalidMulticodecLabel{"invalid-multicodec"};
}

std::string_view ipfs::GetName(Cdc c) {
  switch (c) {
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
Cdc ipfs::Validate(Cdc c) {
  if (GetName(c) == InvalidMulticodecLabel) {
    return Cdc::INVALID;
  }
  return c;
}