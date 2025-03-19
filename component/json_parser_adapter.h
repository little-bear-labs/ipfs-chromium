#ifndef IPFS_CHROMIUM_JSON_PARSER_ADAPTER_H
#define IPFS_CHROMIUM_JSON_PARSER_ADAPTER_H

#include <ipfs_client/ctx/json_parser.h>

namespace ipfs {

/*!
 * \brief Use the Chromium JSON parser, but adapted to the ipfs_client base class
 */
class JsonParserAdapter final : public ctx::JsonParser {
 public:
  ~JsonParserAdapter() noexcept override {}

  /*!
   * \brief Parse a JSON string into a DAG JSON node
   * \param j_str The string representation of JSON
   * \return Ownership pointer to the new node
   */
  std::unique_ptr<DagJsonValue> Parse(std::string_view j_str) override;
};
}

#endif  // IPFS_CHROMIUM_JSON_PARSER_ADAPTER_H
