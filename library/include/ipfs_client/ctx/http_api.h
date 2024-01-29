#ifndef IPFS_CHROMIUM_HTTP_API_H
#define IPFS_CHROMIUM_HTTP_API_H

#include <ipfs_client/http_request_description.h>
#include <functional>

namespace ipfs::ctx {
class HttpApi {
 public:
  using HttpRequestDescription = ::ipfs::HttpRequestDescription;
  using HeaderAccess = std::function<std::string(std::string_view)>;
  using HttpCompleteCallback =
      std::function<void(std::int16_t, std::string_view, HeaderAccess)>;
  virtual void SendHttpRequest(HttpRequestDescription,
                               HttpCompleteCallback cb) const = 0;

  virtual ~HttpApi() noexcept {}
};
}  // namespace ipfs::ctx

#endif  // IPFS_CHROMIUM_HTTP_API_H
