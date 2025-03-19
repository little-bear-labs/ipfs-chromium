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

  /*!
   * \brief Send an HTTP request
   * \param desc Describe the request to be sent
   * \param cb Called with results when the request has completed
   * \return An object which can be used to cancel a pending request
   */
  Canceller SendHttpRequest(ReqDesc desc, OnComplete cb) const override;

  /*!
   * \brief construct
   * \param delegate Loader factor to use for access to HTTP
   */
  ChromiumHttp(network::mojom::URLLoaderFactory& delegate);
};
}  // namespace ipfs

#endif  // IPFS_CHROMIUM_CHROMIUM_HTTP_H
