#ifndef IPFS_CHROMIUM_JSON_PARSER_H
#define IPFS_CHROMIUM_JSON_PARSER_H

#include <memory>
#include <string_view>

namespace ipfs {
class DagJsonValue;
}

namespace ipfs::ctx {
class JsonParser {
 public:
  virtual ~JsonParser() noexcept {}
  virtual std::unique_ptr<DagJsonValue> Parse(std::string_view) = 0;
};
}  // namespace ipfs::ctx

#endif  // IPFS_CHROMIUM_JSON_PARSER_H
