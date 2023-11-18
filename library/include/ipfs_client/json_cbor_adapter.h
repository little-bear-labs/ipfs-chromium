#ifndef IPFS_JSON_CBOR_ADAPTER_H_
#define IPFS_JSON_CBOR_ADAPTER_H_

#include <ipfs_client/context_api.h>
#include <libp2p/multi/content_identifier_codec.hpp>

#if __has_include(<nlohmann/json.hpp>)

#include <nlohmann/json.hpp>
#define HAS_JSON_CBOR_ADAPTER 1

namespace ipfs {
class JsonCborAdapter final : public DagCborValue {
  using CidCodec = libp2p::multi::ContentIdentifierCodec;
  nlohmann::json data_;

 public:
  JsonCborAdapter(nlohmann::json data) : data_{data} {
    if (data_.is_array() && data_.size() == 1UL) {
      data_ = data_[0];
    }
  }
  std::unique_ptr<DagCborValue> at(std::string_view k) const override {
    if (data_.is_object() && data_.contains(k)) {
      return std::make_unique<JsonCborAdapter>(data_.at(k));
    }
    return {};
  }
  std::optional<std::uint64_t> as_unsigned() const override {
    if (data_.is_number_unsigned()) {
      return data_.get<std::uint64_t>();
    }
    return std::nullopt;
  }
  std::optional<std::int64_t> as_signed() const {
    if (data_.is_number_integer()) {
      return data_.get<std::int64_t>();
    } else if (auto ui = as_unsigned()) {
      if (*ui <= std::numeric_limits<std::int64_t>::max()) {
        return static_cast<std::int64_t>(*ui);
      }
    }
    return std::nullopt;
  }
  std::optional<double> as_float() const override {
    if (data_.is_number_float()) {
      return data_.get<double>();
    }
    return std::nullopt;
  }
  std::optional<std::string> as_string() const override {
    if (data_.is_string()) {
      return data_.get<std::string>();
    }
    return std::nullopt;
  }
  std::optional<bool> as_bool() const override {
    if (data_.is_boolean()) {
      return data_.get<bool>();
    }
    return std::nullopt;
  }
  std::optional<std::vector<std::uint8_t>> as_bytes() const override {
    if (data_.is_binary()) {
      return data_.get_binary();
    }
    return std::nullopt;
  }
  std::optional<Cid> as_link() const override {
    if (!data_.is_binary()) {
      return std::nullopt;
    }
    auto& bin = data_.get_binary();
    if (!bin.has_subtype() ||  bin.subtype() != 42) {
      return std::nullopt;
    }
    if (bin.size() < 6) {
      return std::nullopt;
    }
    if (bin[0]) {
      return std::nullopt;
    }
    auto p = reinterpret_cast<std::byte const*>(bin.data()) + 1UL;
    auto result = CidCodec::decode({p, bin.size()-1UL});
    if (result.has_value()) {
      return result.value();
    } else {
      return std::nullopt;
    }
  }
  bool is_map() const override {return data_.is_object();}
  bool is_array() const override {return data_.is_array();}
  void iterate_map(MapElementCallback cb) const override {
    if (!is_map()) {
      return;
    }
    for (auto& [k,v] : data_.items()) {
      JsonCborAdapter el(v);
      cb(k, el);
    }
  }
  void iterate_array(ArrayElementCallback cb) const override {
    if (!is_array()) {
      return;
    }
    for (auto& v : data_) {
      JsonCborAdapter el(v);
      cb(el);
    }
  }
};
}  // namespace ipfs

#endif

#endif  // IPFS_JSON_CBOR_ADAPTER_H_
