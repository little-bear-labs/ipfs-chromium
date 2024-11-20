#ifndef IPFS_CHROMIUM_NLOHMANN_CBOR_PARSER_H
#define IPFS_CHROMIUM_NLOHMANN_CBOR_PARSER_H

#include "cbor_parser.h"

#include <ipfs_client/json_cbor_adapter.h>

#if HAS_JSON_CBOR_ADAPTER

namespace ipfs::ctx {
class NlohmannCborParser final : public CborParser {
 public:
  std::unique_ptr<DagCborValue> Parse(ByteView) override;
  ~NlohmannCborParser() noexcept override {}
};
}  // namespace ipfs::ctx

#endif  // HAS_CBOR_CBOR_ADAPTER
#endif  // IPFS_CHROMIUM_NLOHMANN_CBOR_PARSER_H
