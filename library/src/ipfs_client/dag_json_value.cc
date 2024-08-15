#include <ipfs_client/dag_json_value.h>

#include <ipfs_client/cid.h>

using Self = ipfs::DagJsonValue;

Self::~DagJsonValue() noexcept = default;
auto Self::get_if_link() const -> std::optional<Cid> {
  auto slash = (*this)["/"];
  if (!slash) {
    return std::nullopt;
  }
  auto str = slash->get_if_string();
  if (!str) {
    return std::nullopt;
  }
  auto cid = Cid(*str);
  if (cid.valid()) {
    return cid;
  }
  return std::nullopt;
}