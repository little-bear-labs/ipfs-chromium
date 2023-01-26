#ifndef CHROMIUM_IPFS_URL_LOADER_FACTORY_H_
#define CHROMIUM_IPFS_URL_LOADER_FACTORY_H_ 1

#include "services/network/public/cpp/self_deleting_url_loader_factory.h"
#include "base/component_export.h"

namespace content {
  class BrowserContext;
}

namespace ipfs {
  enum class Scheme
  {
    IPFS,
    IPNS
  };
  class COMPONENT_EXPORT(IPFS) LoaderFactory final : public network::SelfDeletingURLLoaderFactory {
    void CreateLoaderAndStart(
          mojo::PendingReceiver<network::mojom::URLLoader> receiver
        , int32_t request_id
        , uint32_t options
        , network::ResourceRequest const& resource_request
        , mojo::PendingRemote<network::mojom::URLLoaderClient> client
        , net::MutableNetworkTrafficAnnotationTag const& traffic_annotation
        ) override;
  public:
    LoaderFactory(mojo::PendingReceiver<network::mojom::URLLoaderFactory>&& receiver);

    static mojo::PendingRemote<network::mojom::URLLoaderFactory> Create(
        int frame_tree_node_id
      , content::BrowserContext* browser_context
      , Scheme
      );
  };

  template<class Map>
  void registerFactories(
        Map* factories
      , int frame_tree_node_id
      , content::BrowserContext* browser_context
      )
  {
    DCHECK(factories);
    if (!factories) {
      return;
    }
    factories->emplace("ipfs", LoaderFactory::Create(frame_tree_node_id, browser_context, Scheme::IPFS));
    factories->emplace("ipns", LoaderFactory::Create(frame_tree_node_id, browser_context, Scheme::IPNS));
  }
}

#endif // CHROMIUM_IPFS_URL_LOADER_FACTORY_H
