#pragma once

#include <ipfs_client/dag_cbor_value.h>

#include <gtest/gtest.h>

#include <vector>

namespace {
struct MockCborVal : public ipfs::DagCborValue {
  std::vector<MockCborVal> list;
  std::optional<std::int64_t> number;
  std::optional<double> flt;
  std::optional<std::string> str;
  std::optional<bool> bol;
  std::map<std::string,MockCborVal> dict;
  std::unique_ptr<ipfs::DagCborValue> at(std::string_view k) const {
    auto i = dict.find(std::string{k});
    if (i == dict.end()) {
      return {};
    }
    return std::make_unique<MockCborVal>(i->second);
  }
  std::optional<std::uint64_t> as_unsigned() const {
    return std::nullopt;
  }
  std::optional<double> as_float() const  {
    return flt;
  }
  std::optional<std::int64_t> as_signed() const {
    return number;
  }
  std::optional<std::string> as_string() const  {return str;}
  std::optional<std::vector<std::uint8_t>> as_bytes() const  {return std::nullopt;}
   std::optional<bool> as_bool() const  {
    return bol;
  }
   std::optional<ipfs::Cid> as_link() const  {
    return {};
  }
   bool is_map() const  {
    return ! dict.empty();
  }
   bool is_array() const  {return!list.empty();}
   void iterate_map(MapElementCallback f) const  {
     for (auto& [k,v] : dict) {
       f(k,v);
     }
   }
   void iterate_array(ArrayElementCallback f) const  {
     for (auto& v : list) {
       f(v);
     }
   }
  ~MockCborVal() noexcept = default;
};
}
