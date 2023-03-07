#include "url_loader_factory.h"

#include "inter_request_state.h"
#include "ipfs_url_loader.h"
#include "ipns_url_loader.h"

void ipfs::IpfsURLLoaderFactory::Create(
    NonNetworkURLLoaderFactoryMap* in_out,
    content::BrowserContext* context,
    URLLoaderFactory* default_factory,
    network::mojom::NetworkContext* net_ctxt) {
  for (char const* scheme : {"ipfs", "ipns"}) {
    mojo::PendingRemote<network::mojom::URLLoaderFactory> pending;
    new IpfsURLLoaderFactory(scheme, pending.InitWithNewPipeAndPassReceiver(),
                             context, default_factory, net_ctxt);
    in_out->emplace(scheme, std::move(pending));
  }
}

ipfs::IpfsURLLoaderFactory::IpfsURLLoaderFactory(
    std::string scheme,
    mojo::PendingReceiver<network::mojom::URLLoaderFactory> factory_receiver,
    content::BrowserContext* context,
    URLLoaderFactory* default_factory,
    network::mojom::NetworkContext* net_ctxt)
    : network::SelfDeletingURLLoaderFactory(std::move(factory_receiver)),
      scheme_{scheme},
      context_{context},
      default_factory_{default_factory},
      network_context_{net_ctxt} {}

ipfs::IpfsURLLoaderFactory::~IpfsURLLoaderFactory() noexcept {}

void ipfs::IpfsURLLoaderFactory::CreateLoaderAndStart(
    mojo::PendingReceiver<network::mojom::URLLoader> loader,
    int32_t /*request_id*/,
    uint32_t /*options*/,
    network::ResourceRequest const& request,
    mojo::PendingRemote<network::mojom::URLLoaderClient> client,
    net::MutableNetworkTrafficAnnotationTag const&  // traffic_annotation
) {
  LOG(INFO) << "IPFS subresource: case=" << scheme_
            << " url=" << request.url.spec();
  DCHECK(default_factory_);
  if (scheme_ == "ipfs") {
    auto ptr = std::make_shared<IpfsUrlLoader>(
        *default_factory_, InterRequestState::FromBrowserContext(context_));
    ptr->StartRequest(ptr, request, std::move(loader), std::move(client));
  } else if (scheme_ == "ipns") {
    auto ptr = std::make_shared<IpnsUrlLoader>(
        InterRequestState::FromBrowserContext(context_), request.url.host(),
        network_context_, *default_factory_);
    ptr->StartHandling(ptr, request, std::move(loader), std::move(client));

  } else {
    NOTREACHED();
  }
}
