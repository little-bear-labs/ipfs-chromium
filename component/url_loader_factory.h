#ifndef IPFS_URL_LOADER_FACTORY_H_
#define IPFS_URL_LOADER_FACTORY_H_

#include "services/network/public/cpp/self_deleting_url_loader_factory.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"

#include <string>

namespace content {
class BrowserContext;
}
namespace network {
namespace mojom {
class NetworkContext;
}
}  // namespace network

namespace ipfs {
using NonNetworkURLLoaderFactoryMap =
    std::map<std::string,
             mojo::PendingRemote<network::mojom::URLLoaderFactory>>;

class COMPONENT_EXPORT(IPFS) IpfsURLLoaderFactory
    : public network::SelfDeletingURLLoaderFactory {
 public:
  static void Create(NonNetworkURLLoaderFactoryMap* in_out,
                     content::BrowserContext*,
                     URLLoaderFactory*,
                     network::mojom::NetworkContext*);

 private:
  IpfsURLLoaderFactory(std::string,
                       mojo::PendingReceiver<network::mojom::URLLoaderFactory>,
                       content::BrowserContext*,
                       network::mojom::URLLoaderFactory*,
                       network::mojom::NetworkContext*);
  ~IpfsURLLoaderFactory() noexcept override;
  void CreateLoaderAndStart(
      mojo::PendingReceiver<network::mojom::URLLoader> loader,
      int32_t request_id,
      uint32_t options,
      network::ResourceRequest const& request,
      mojo::PendingRemote<network::mojom::URLLoaderClient> client,
      net::MutableNetworkTrafficAnnotationTag const& traffic_annotation)
      override;

  std::string scheme_;
  content::BrowserContext* context_;
  network::mojom::URLLoaderFactory* default_factory_;
  network::mojom::NetworkContext* network_context_;
};
}  // namespace ipfs

#endif  // IPFS_URL_LOADER_FACTORY_H_
