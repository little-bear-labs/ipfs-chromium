#ifndef IPFS_MOCK_BOR_H_
#define IPFS_MOCK_BOR_H_

#include "ipfs_client/client.h"

#include <gtest/gtest.h>

namespace i = ipfs;
using namespace std::literals;

namespace {
struct MockCbor final : public i::DagCborValue {
  std::optional<std::string> sval = std::nullopt;
  std::optional<std::uint64_t> uval = std::nullopt;
  std::map<std::string, MockCbor> children;
  std::unique_ptr<DagCborValue> at(std::string_view key) const {
    std::string k{key};
    auto i = children.find(k);
    if (children.end() == i) {
      return {};
    }
    return std::make_unique<MockCbor>(i->second);
  }
  std::optional<std::uint64_t> as_unsigned() const { return uval; }
  std::optional<std::string> as_string() const { return sval; }
  std::optional<i::Cid> as_link() const { return {};
  }
  std::optional<bool> as_bool() const { return {}; }
  std::optional<std::int64_t> as_signed() const { return {}; }
  std::optional<double> as_float() const { return {}; }
  std::optional<std::vector<std::uint8_t>> as_bytes() const {
    if (sval.has_value()) {
      auto b = reinterpret_cast<std::uint8_t const*>(sval.value().data());
      auto e = b + sval.value().size();
      return std::vector<std::uint8_t>(b, e);
    }
    return std::nullopt;
  }
  void set(std::string k, MockCbor v) { children[k] = v; }
  MockCbor(std::string s) : sval{s} {}
  MockCbor(std::uint64_t u) : uval{u} {}
  MockCbor() = default;
  MockCbor(MockCbor const&) = default;
  std::string html() const { return "<p>no</p>"; }
  bool is_map() const { return false; }
  bool is_array() const { return false; }
  void iterate_map(MapElementCallback) const {}
  void iterate_array(ArrayElementCallback) const {}
};
}  // namespace

#endif  // IPFS_MOCK_BOR_H_
