#include "chromium_cbor_adapter.h"

#include <libp2p/multi/content_identifier_codec.hpp>

#include <base/logging.h>

using Self = ipfs::ChromiumCborAdapter;
using CidCodec = libp2p::multi::ContentIdentifierCodec;

bool Self::is_map() const {
  return cbor_.is_map();
}
bool Self::is_array() const {
  return cbor_.is_array();
}
auto Self::at(std::string_view key) const -> std::unique_ptr<DagCborValue> {
  if (is_map()) {
    auto& m = cbor_.GetMap();
    auto it = m.find(cbor::Value{base::StringPiece{key}});
    if (m.end() != it) {
      return std::make_unique<Self>(it->second.Clone());
    }
  }
  return {};
}
std::optional<std::uint64_t> Self::as_unsigned() const {
  if (cbor_.is_unsigned()) {
    return cbor_.GetUnsigned();
  }
  return std::nullopt;
}
std::optional<std::int64_t> Self::as_signed() const {
  if (cbor_.is_integer()) {
    return cbor_.GetInteger();
  }
  return {};
}
std::optional<double> Self::as_float() const {
  return {};
}

std::optional<std::string> Self::as_string() const {
  if (cbor_.is_string()) {
    return cbor_.GetString();
  }
  return std::nullopt;
}
auto Self::as_bytes() const -> std::optional<std::vector<std::uint8_t>> {
  if (cbor_.is_bytestring()) {
    return cbor_.GetBytestring();
  }
  return std::nullopt;
}
auto Self::as_link() const -> std::optional<Cid> {
  VLOG(1) << "Trying to do an as_link(" << static_cast<int>(cbor_.type()) << ',' << std::boolalpha << cbor_.has_tag() << ")";
  if (!cbor_.has_tag() || cbor_.GetTag() != 42UL || !cbor_.is_bytestring()) {
    VLOG(1) << "This is not a link.";
    return std::nullopt;
  }
  auto& bytes = cbor_.GetBytestring();
  auto* byte_ptr = reinterpret_cast<Byte const*>(bytes.data()) + 1;
  auto result = CidCodec::decode({byte_ptr, bytes.size() - 1UL});
  if (result.has_value()) {
    return result.value();
  } else {
    LOG(ERROR) << "Unable to decode bytes from DAG-CBOR Link as CID.";
    return std::nullopt;
  }
}
std::optional<bool> Self::as_bool() const {
  if (cbor_.is_bool()) {
    return cbor_.GetBool();
  }
  return std::nullopt;
}
void Self::iterate_map(MapElementCallback cb) const {
  auto& m = cbor_.GetMap();
  for (auto& [k,v] : m) {
    cb(k.GetString(), Self{v});
  }
}
void Self::iterate_array(ArrayElementCallback cb) const {
  auto& a = cbor_.GetArray();
  for (auto& e : a) {
    cb(Self{e});
  }
}

Self::ChromiumCborAdapter(cbor::Value const& v) : cbor_{v.Clone()} {}
Self::ChromiumCborAdapter(cbor::Value&& v) : cbor_{std::move(v)} {}
Self::ChromiumCborAdapter(ChromiumCborAdapter const& rhs)
    : cbor_{rhs.cbor_.Clone()} {}

Self::~ChromiumCborAdapter() noexcept {}
