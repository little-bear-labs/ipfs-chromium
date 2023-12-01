#ifndef IPFS_CHROMIUM_JSON_ADAPTER_H_
#define IPFS_CHROMIUM_JSON_ADAPTER_H_

#include <base/values.h>
#include <ipfs_client/dag_json_value.h>

namespace ipfs {
class ChromiumJsonAdapter final : public ipfs::DagJsonValue {
  base::Value data_;
  std::string pretty_print() const override;
  std::unique_ptr<DagJsonValue> operator[](std::string_view) const override;
  std::optional<std::string> get_if_string() const override;
  std::optional<std::vector<std::string>> object_keys() const override;
  bool iterate_list(std::function<void(DagJsonValue const&)>) const override;

 public:
  ChromiumJsonAdapter(base::Value);
  ~ChromiumJsonAdapter() noexcept override;
};
}  // namespace ipfs

#endif  // IPFS_CHROMIUM_JSON_ADAPTER_H_
