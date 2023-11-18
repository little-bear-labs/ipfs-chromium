#ifndef IPFS_DAG_CBOR_VALUE_H_
#define IPFS_DAG_CBOR_VALUE_H_

#include <libp2p/multi/content_identifier.hpp>

#include <optional>
#include <string_view>
#include <vector>

namespace ipfs {
class DagCborValue {
 public:
  virtual std::unique_ptr<DagCborValue> at(std::string_view) const = 0;
  virtual std::optional<std::uint64_t> as_unsigned() const = 0;
  virtual std::optional<std::int64_t> as_signed() const = 0;
  virtual std::optional<double> as_float() const = 0;
  virtual std::optional<std::string> as_string() const = 0;
  virtual std::optional<std::vector<std::uint8_t>> as_bytes() const = 0;
  virtual std::optional<bool> as_bool() const = 0;
  using Cid = libp2p::multi::ContentIdentifier;
  virtual std::optional<Cid> as_link() const = 0;
  virtual bool is_map() const = 0;
  virtual bool is_array() const = 0;
  using MapElementCallback = std::function<void(std::string_view,DagCborValue const&)>;
  using ArrayElementCallback = std::function<void(DagCborValue const&)>;
  virtual void iterate_map(MapElementCallback) const = 0;
  virtual void iterate_array(ArrayElementCallback) const = 0;
  std::string html() const;
  void html(std::ostream&) const;
  virtual ~DagCborValue() noexcept {}
};
}

#endif  // IPFS_DAG_CBOR_VALUE_H_
