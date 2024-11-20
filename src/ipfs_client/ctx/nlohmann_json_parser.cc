#include <ipfs_client/ctx/nlohmann_json_parser.h>

#include <string_view>
#include <memory>
#include <sstream>
#include <iomanip>

#if HAS_JSON_CBOR_ADAPTER

#include "log_macros.h"

using Self = ipfs::ctx::NlohmannJsonParser;

auto Self::Parse(std::string_view j_str)
    -> std::unique_ptr<DagJsonValue> {
  auto data = nlohmann::json::parse(j_str);
  std::ostringstream oss;
  oss << std::setw(2) << data;
  VLOG(1) << "Parsed " << j_str.size() << " bytes of JSON string and got "
          << oss.str();
  return std::make_unique<ipfs::JsonCborAdapter>(data);
}

#endif
