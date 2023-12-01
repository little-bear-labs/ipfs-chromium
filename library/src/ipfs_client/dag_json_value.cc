#include <ipfs_client/dag_json_value.h>

#include <libp2p/multi/content_identifier_codec.hpp>

using Self = ipfs::DagJsonValue;
using CidCodec = libp2p::multi::ContentIdentifierCodec;

Self::~DagJsonValue() noexcept {}
auto Self::get_if_link() const -> std::optional<Cid> {
  auto slash = (*this)["/"];
  if (!slash) {
    return std::nullopt;
  }
  auto str = slash->get_if_string();
  if (!str) {
    return std::nullopt;
  }
  auto cid = CidCodec::fromString(*str);
  if (cid.has_value()) {
    return cid.value();
  }
  return std::nullopt;
}