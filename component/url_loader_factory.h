#ifndef IPFS_URL_LOADER_FACTORY_H_
#define IPFS_URL_LOADER_FACTORY_H_

#include "services/network/public/cpp/self_deleting_url_loader_factory.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"

#include <string>

namespace content {
class BrowserContext;
}

namespace ipfs {
using NonNetworkURLLoaderFactoryMap =
    std::map<std::string,
             mojo::PendingRemote<network::mojom::URLLoaderFactory>>;

class COMPONENT_EXPORT(IPFS) IpfsURLLoaderFactory
    : public network::SelfDeletingURLLoaderFactory {
 public:
  static void Create(NonNetworkURLLoaderFactoryMap* in_out,
                     std::string from,
                     content::BrowserContext*,
                     URLLoaderFactory*);
  IpfsURLLoaderFactory(std::string,
                       mojo::PendingReceiver<network::mojom::URLLoaderFactory>,
                       content::BrowserContext*,
                       network::mojom::URLLoaderFactory*);
  ~IpfsURLLoaderFactory() noexcept override;
  void CreateLoaderAndStart(
      mojo::PendingReceiver<network::mojom::URLLoader> loader,
      int32_t request_id,
      uint32_t options,
      network::ResourceRequest const& request,
      mojo::PendingRemote<network::mojom::URLLoaderClient> client,
      net::MutableNetworkTrafficAnnotationTag const& traffic_annotation)
      override;

 private:
  std::string debugging_name_;
  content::BrowserContext* context_;
  network::mojom::URLLoaderFactory* default_factory_;
};
}  // namespace ipfs

#endif  // IPFS_URL_LOADER_FACTORY_H_
