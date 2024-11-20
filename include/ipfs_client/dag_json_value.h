#ifndef IPFS_DAG_JSON_VALUE_H_
#define IPFS_DAG_JSON_VALUE_H_

#include <ipfs_client/cid.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace ipfs {
/*! A value in the DOM of a dag-json document
 */
class DagJsonValue {
 public:
  virtual std::string pretty_print() const = 0;
  virtual std::unique_ptr<DagJsonValue> operator[](std::string_view) const = 0;
  virtual std::optional<std::string> get_if_string() const = 0;
  virtual std::optional<std::vector<std::string>> object_keys() const = 0;
  virtual bool iterate_list(std::function<void(DagJsonValue const&)>) const = 0;
  virtual ~DagJsonValue() noexcept;

  std::optional<Cid> get_if_link() const;
};
}  // namespace ipfs

#endif  // IPFS_DAG_JSON_VALUE_H_
