#ifndef IPFS_CHROMIUM_CBOR_ADAPTER_H_
#define IPFS_CHROMIUM_CBOR_ADAPTER_H_

#include <components/cbor/values.h>

#include <ipfs_client/ctx/cbor_parser.h>
#include <ipfs_client/dag_cbor_value.h>

namespace ipfs {
/*! Adapting Chromium's components/cbor to the API needed by ipfs_client
 */
class ChromiumCborAdapter final : public DagCborValue, public ctx::CborParser {
  cbor::Value cbor_;

  std::unique_ptr<DagCborValue> at(std::string_view) const override;
  std::optional<std::uint64_t> as_unsigned() const override;
  std::optional<std::int64_t> as_signed() const override;
  std::optional<double> as_float() const override;
  std::optional<std::string> as_string() const override;
  std::optional<std::vector<std::uint8_t>> as_bytes() const override;
  std::optional<Cid> as_link() const override;
  std::optional<bool> as_bool() const override;

  /*! \return Whether this represents a map/dict
   */
  bool is_map() const override;

  /*! \return Whether this represents an array/list
   */
  bool is_array() const override;
  void iterate_map(MapElementCallback) const override;
  void iterate_array(ArrayElementCallback) const override;

 public:
  explicit ChromiumCborAdapter();

  /*! \brief Move ctor
   */
  ChromiumCborAdapter(cbor::Value&&);
  ChromiumCborAdapter(cbor::Value const&);
  ChromiumCborAdapter(ChromiumCborAdapter const& rhs);
  ~ChromiumCborAdapter() noexcept override;

  /*!
   * \brief Parse CBOR bytes
   * \param bytes to parse
   * \return The DAG CBOR IPLD node
   */
  std::unique_ptr<DagCborValue> Parse(ByteView bytes) override;
};
}  // namespace ipfs

#endif  // IPFS_CHROMIUM_CBOR_ADAPTER_H_
