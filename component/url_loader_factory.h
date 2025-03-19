#ifndef IPFS_URL_LOADER_FACTORY_H_
#define IPFS_URL_LOADER_FACTORY_H_

#include "services/network/public/cpp/self_deleting_url_loader_factory.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"

#include <string>

class PrefService;
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
  /*!
   * \brief factory method
   * \param in_out Where to put the newly created object
   */
  static void Create(NonNetworkURLLoaderFactoryMap* in_out,
                     content::BrowserContext* context,
                     URLLoaderFactory* default_factory,
                     network::mojom::NetworkContext* net_ctxt,
                     PrefService* pref_svc);

 private:
  IpfsURLLoaderFactory(std::string,
                       mojo::PendingReceiver<network::mojom::URLLoaderFactory>,
                       content::BrowserContext*,
                       network::mojom::URLLoaderFactory*,
                       network::mojom::NetworkContext*,
                       PrefService*);
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
  raw_ptr<content::BrowserContext> context_;
  raw_ptr<network::mojom::URLLoaderFactory> default_factory_;
  raw_ptr<network::mojom::NetworkContext> network_context_;
};
}  // namespace ipfs

#endif  // IPFS_URL_LOADER_FACTORY_H_
