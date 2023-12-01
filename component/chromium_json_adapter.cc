#include "chromium_json_adapter.h"

using Self = ipfs::ChromiumJsonAdapter;

Self::ChromiumJsonAdapter(base::Value d) : data_(std::move(d)) {}
Self::~ChromiumJsonAdapter() noexcept {}
std::string Self::pretty_print() const {
  return data_.DebugString();
}
std::optional<std::string> Self::get_if_string() const {
  auto* s = data_.GetIfString();
  if (s) {
    return *s;
  } else {
    return std::nullopt;
  }
}
auto Self::operator[](std::string_view k) const
    -> std::unique_ptr<DagJsonValue> {
  if (auto* m = data_.GetIfDict()) {
    if (auto* v = m->Find(k)) {
      return std::make_unique<Self>(v->Clone());
    }
  }
  return {};
}
bool Self::iterate_list(std::function<void(DagJsonValue const&)> cb) const {
  auto* l = data_.GetIfList();
  if (!l) {
    return false;
  }
  for (auto& v : *l) {
    Self wrap(v.Clone());
    cb(wrap);
  }
  return true;
}
std::optional<std::vector<std::string>> Self::object_keys() const {
  auto* m = data_.GetIfDict();
  if (!m) {
    return std::nullopt;
  }
  std::vector<std::string> rv;
  for (auto [k, v] : *m) {
    rv.push_back(k);
  }
  return rv;
}