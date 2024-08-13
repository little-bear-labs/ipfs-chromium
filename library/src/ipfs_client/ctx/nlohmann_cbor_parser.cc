#include <ipfs_client/ctx/nlohmann_cbor_parser.h>

#if HAS_JSON_CBOR_ADAPTER

using Self = ipfs::ctx::NlohmannCborParser;

auto Self::Parse(ipfs::ByteView bytes) -> std::unique_ptr<DagCborValue> {
  auto data = nlohmann::json::from_cbor(
      bytes, false, true, nlohmann::detail::cbor_tag_handler_t::store);
  return std::make_unique<ipfs::JsonCborAdapter>(data);
}

#endif
