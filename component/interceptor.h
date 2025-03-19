#ifndef IPFS_INTERCEPTOR_H_
#define IPFS_INTERCEPTOR_H_

#include "content/public/browser/url_loader_request_interceptor.h"

class PrefService;
namespace network::mojom {
class URLLoaderFactory;
class NetworkContext;
}  // namespace network::mojom

namespace ipfs {

class COMPONENT_EXPORT(IPFS) Interceptor final
    : public content::URLLoaderRequestInterceptor {
  raw_ptr<network::mojom::URLLoaderFactory> loader_factory_;
  raw_ptr<network::mojom::NetworkContext> network_context_;
  raw_ptr<PrefService> pref_svc_;

  /*!
   * \brief Asynchronously check whether an IpfsUrlLoader would be appropriate, and if so create one
   * \param req The request to be loaded
   * \param context In what browser context
   * \param loader_callback What to call with a loader or lack thereof
   */
  void MaybeCreateLoader(network::ResourceRequest const& req,
                         content::BrowserContext* context,
                         LoaderCallback loader_callback) override;

 public:
  Interceptor(network::mojom::URLLoaderFactory* handles_http,
              network::mojom::NetworkContext*);
};
}  // namespace ipfs

#endif  // IPFS_INTERCEPTOR_H_
