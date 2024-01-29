#include "json_parser_adapter.h"

#include "chromium_json_adapter.h"

#include <base/json/json_reader.h>

using Self = ipfs::JsonParserAdapter;

auto Self::Parse(std::string_view j_str)  -> std::unique_ptr<ipfs::DagJsonValue> {
  auto d = base::JSONReader::Read(j_str, base::JSON_ALLOW_TRAILING_COMMAS);
     if (d) {
       return std::make_unique<ChromiumJsonAdapter>(std::move(d.value()));
     }
     return {};
}
