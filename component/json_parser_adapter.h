#ifndef IPFS_CHROMIUM_JSON_PARSER_ADAPTER_H
#define IPFS_CHROMIUM_JSON_PARSER_ADAPTER_H

#include <ipfs_client/ctx/json_parser.h>

namespace ipfs {
class JsonParserAdapter final : public ctx::JsonParser {
 public:
  ~JsonParserAdapter() noexcept override {}
  std::unique_ptr<DagJsonValue> Parse(std::string_view) override;
};
}

#endif  // IPFS_CHROMIUM_JSON_PARSER_ADAPTER_H
