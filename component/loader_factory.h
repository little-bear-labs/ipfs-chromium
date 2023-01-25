#ifndef CHROMIUM_IPFS_URL_LOADER_FACTORY_H_
#define CHROMIUM_IPFS_URL_LOADER_FACTORY_H_ 1

#include "services/network/url_loader_factory.h"

namespace ipfs {
  class LoaderFactory final : public network::URLLoaderFactory {
    void CreateLoaderAndStart(
          mojo::PendingReceiver<network::mojom::URLLoader> receiver
        , int32_t request_id
        , uint32_t options
        , network::ResourceRequest const& resource_request
        , mojo::PendingRemote<network::mojom::URLLoaderClient> client
        , net::MutableNetworkTrafficAnnotationTag const& traffic_annotation
        ) override;
  public:
    LoaderFactory();
  };
}

#endif // CHROMIUM_IPFS_URL_LOADER_FACTORY_H
