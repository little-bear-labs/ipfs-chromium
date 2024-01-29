#ifndef IPFS_CHROMIUM_CHROMIUM_HTTP_H
#define IPFS_CHROMIUM_CHROMIUM_HTTP_H

#include <ipfs_client/ctx/http_api.h>

#include <vocab/raw_ptr.h>

namespace network::mojom {
class URLLoaderFactory;
}  // namespace network::mojom

namespace ipfs {
class ChromiumHttp : public ctx::HttpApi {
  raw_ptr<network::mojom::URLLoaderFactory> loader_factory_ = nullptr;

 public:
  void SendHttpRequest(HttpRequestDescription,
                       HttpCompleteCallback cb) const override;
  ChromiumHttp(network::mojom::URLLoaderFactory&);
};
}  // namespace ipfs

#endif  // IPFS_CHROMIUM_CHROMIUM_HTTP_H
