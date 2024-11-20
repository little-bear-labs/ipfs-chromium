#ifndef IPFS_HTTP_REQUEST_DESCRIPTION_H_
#define IPFS_HTTP_REQUEST_DESCRIPTION_H_

#include <optional>
#include <string>

#include <cstdint>

namespace ipfs {
/*! Describe an HTTP request to be made, agnostic to how that's done
 */
struct HttpRequestDescription {
  std::string url;
  int timeout_seconds;
  std::string accept;
  std::optional<std::size_t> max_response_size;
  bool operator==(HttpRequestDescription const&) const;
  bool operator<(HttpRequestDescription const&) const;
};
}  // namespace ipfs

#endif  // IPFS_HTTP_REQUEST_DESCRIPTION_H_
