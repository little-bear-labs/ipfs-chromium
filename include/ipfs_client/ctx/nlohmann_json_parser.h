#ifndef IPFS_CHROMIUM_NLOHMANN_JSON_PARSER_H
#define IPFS_CHROMIUM_NLOHMANN_JSON_PARSER_H

#include "json_parser.h"

#include <ipfs_client/json_cbor_adapter.h>

#if HAS_JSON_CBOR_ADAPTER

namespace ipfs::ctx {
class NlohmannJsonParser : public JsonParser {
 public:
  std::unique_ptr<DagJsonValue> Parse(std::string_view) override;
  ~NlohmannJsonParser() noexcept override {}
};
}  // namespace ipfs::ctx

#endif  // HAS_JSON_CBOR_ADAPTER
#endif  // IPFS_CHROMIUM_NLOHMANN_JSON_PARSER_H
