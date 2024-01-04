#include "url_loader_factory.h"

#include "inter_request_state.h"
#include "ipfs_url_loader.h"

void ipfs::IpfsURLLoaderFactory::Create(
    NonNetworkURLLoaderFactoryMap* in_out,
    content::BrowserContext* context,
    URLLoaderFactory* default_factory,
    network::mojom::NetworkContext* net_ctxt,
    PrefService* pref_svc) {
  for (char const* scheme : {"ipfs", "ipns"}) {
    mojo::PendingRemote<network::mojom::URLLoaderFactory> pending;
    new IpfsURLLoaderFactory(scheme, pending.InitWithNewPipeAndPassReceiver(),
                             context, default_factory, net_ctxt, pref_svc);
    in_out->emplace(scheme, std::move(pending));
  }
}

ipfs::IpfsURLLoaderFactory::IpfsURLLoaderFactory(
    std::string scheme,
    mojo::PendingReceiver<network::mojom::URLLoaderFactory> factory_receiver,
    content::BrowserContext* context,
    URLLoaderFactory* default_factory,
    network::mojom::NetworkContext* net_ctxt,
    PrefService* pref_svc)
    : network::SelfDeletingURLLoaderFactory(std::move(factory_receiver)),
      scheme_{scheme},
      context_{context},
      default_factory_{default_factory},
      network_context_{net_ctxt} {}

ipfs::IpfsURLLoaderFactory::~IpfsURLLoaderFactory() noexcept {
  context_ = nullptr;
  default_factory_ = nullptr;
  network_context_ = nullptr;
}

void ipfs::IpfsURLLoaderFactory::CreateLoaderAndStart(
    mojo::PendingReceiver<network::mojom::URLLoader> loader,
    int32_t /*request_id*/,
    uint32_t /*options*/,
    network::ResourceRequest const& request,
    mojo::PendingRemote<network::mojom::URLLoaderClient> client,
    net::MutableNetworkTrafficAnnotationTag const&  // traffic_annotation
) {
  DCHECK(default_factory_);
  if (scheme_ == "ipfs" || scheme_ == "ipns") {
    auto ptr = std::make_shared<IpfsUrlLoader>(
        *default_factory_, InterRequestState::FromBrowserContext(context_));
    ptr->StartRequest(ptr, request, std::move(loader), std::move(client));
  }
}
