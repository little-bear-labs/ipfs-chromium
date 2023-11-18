#ifndef IPFS_CHROMIUM_CBOR_ADAPTER_H_
#define IPFS_CHROMIUM_CBOR_ADAPTER_H_

#include <components/cbor/values.h>

#include <ipfs_client/context_api.h>

namespace ipfs {
class ChromiumCborAdapter final : public DagCborValue {
  cbor::Value cbor_;

  std::unique_ptr<DagCborValue> at(std::string_view) const override;
  std::optional<std::uint64_t> as_unsigned() const override;
  std::optional<std::int64_t> as_signed() const override;
  std::optional<double> as_float() const override;
  std::optional<std::string> as_string() const override;
  std::optional<std::vector<std::uint8_t>> as_bytes() const override;
  std::optional<Cid> as_link() const override;
  std::optional<bool> as_bool() const override;
  bool is_map() const override;
  bool is_array() const override;
  void iterate_map(MapElementCallback) const override;
  void iterate_array(ArrayElementCallback) const override;

 public:
  ChromiumCborAdapter(cbor::Value&&);
  ChromiumCborAdapter(cbor::Value const&);
  ChromiumCborAdapter(ChromiumCborAdapter const& rhs);
  ~ChromiumCborAdapter() noexcept override;
};
}  // namespace ipfs

#endif  // IPFS_CHROMIUM_CBOR_ADAPTER_H_
