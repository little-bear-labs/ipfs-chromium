#ifndef IPFS_CHROMIUM_CHROMIUM_HTTP_H
#define IPFS_CHROMIUM_CHROMIUM_HTTP_H

#include <ipfs_client/ctx/http_api.h>

#include <vocab/raw_ptr.h>

namespace network::mojom {
class URLLoaderFactory;
}  // namespace network::mojom

namespace ipfs {
/*! Using Chromium's URLLoader mechanisms to issue HTTP requests
 */
class ChromiumHttp : public ctx::HttpApi {
  raw_ptr<network::mojom::URLLoaderFactory> loader_factory_ = nullptr;

 public:
  Canceller SendHttpRequest(ReqDesc, OnComplete cb) const override;
  ChromiumHttp(network::mojom::URLLoaderFactory&);
};
}  // namespace ipfs

#endif  // IPFS_CHROMIUM_CHROMIUM_HTTP_H
