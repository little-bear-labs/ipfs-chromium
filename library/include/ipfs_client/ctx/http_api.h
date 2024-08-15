#ifndef IPFS_CHROMIUM_HTTP_API_H
#define IPFS_CHROMIUM_HTTP_API_H

#include <ipfs_client/http_request_description.h>
#include <functional>

namespace ipfs::ctx {
class HttpApi {
 public:
  using ReqDesc = ::ipfs::HttpRequestDescription;
  using Hdrs = std::function<std::string(std::string_view)>;
  using OnComplete = std::function<void(std::int16_t, std::string_view, Hdrs)>;
  using Canceller = std::function<void()>;
  virtual Canceller SendHttpRequest(ReqDesc, OnComplete cb) const = 0;

  virtual ~HttpApi() noexcept {}
};
}  // namespace ipfs::ctx

#endif  // IPFS_CHROMIUM_HTTP_API_H
