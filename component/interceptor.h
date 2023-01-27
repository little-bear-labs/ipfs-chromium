#ifndef CHROMIUM_IPFS_INTERCEPTOR_H_
#define CHROMIUM_IPFS_INTERCEPTOR_H_

#include "content/public/browser/url_loader_request_interceptor.h"

namespace ipfs {
  class COMPONENT_EXPORT(IPFS) Interceptor final : public content::URLLoaderRequestInterceptor {
    void MaybeCreateLoader(
        network::ResourceRequest const&
      , content::BrowserContext*
      , LoaderCallback
      ) override;

  public:
    Interceptor();
  };
}

#endif // CHROMIUM_IPFS_INTERCEPTOR_H
