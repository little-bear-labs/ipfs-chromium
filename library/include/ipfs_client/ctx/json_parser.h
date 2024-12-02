#ifndef IPFS_CHROMIUM_JSON_PARSER_H
#define IPFS_CHROMIUM_JSON_PARSER_H

#include <memory>
#include <string_view>

namespace ipfs {
class DagJsonValue;
}

namespace ipfs::ctx {
/*! Base class for implementations of JSON parsing
 */
class JsonParser {
 public:
  virtual ~JsonParser() noexcept {}
  /*! @param json The JSON text to parse
   *  @return The DOM object or NULL if parsing failed.
   */
  virtual std::unique_ptr<DagJsonValue> Parse(std::string_view json) = 0;
};
}  // namespace ipfs::ctx

#endif  // IPFS_CHROMIUM_JSON_PARSER_H
