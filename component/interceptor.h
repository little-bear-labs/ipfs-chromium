#ifndef IPFS_INTERCEPTOR_H_
#define IPFS_INTERCEPTOR_H_

#include "content/public/browser/url_loader_request_interceptor.h"

namespace network::mojom {
class URLLoaderFactory;
class NetworkContext;
}  // namespace network::mojom

namespace ipfs {

class COMPONENT_EXPORT(IPFS) Interceptor final
    : public content::URLLoaderRequestInterceptor {
  raw_ptr<network::mojom::URLLoaderFactory> loader_factory_;
  raw_ptr<network::mojom::NetworkContext> network_context_;

  void MaybeCreateLoader(network::ResourceRequest const&,
                         content::BrowserContext*,
                         LoaderCallback) override;

 public:
  Interceptor(network::mojom::URLLoaderFactory* handles_http,
              network::mojom::NetworkContext*);
};
}  // namespace ipfs

#endif  // IPFS_INTERCEPTOR_H_
