#include "url_loader_factory.h"

#include "inter_request_state.h"
#include "loader.h"

#include <iostream>

void ipfs::IpfsURLLoaderFactory::Create(NonNetworkURLLoaderFactoryMap* in_out,
                                        std::string from,
                                        content::BrowserContext* context,
                                        URLLoaderFactory* default_factory) {
  for (char const* scheme : {"ipfs", "ipns"}) {
    mojo::PendingRemote<network::mojom::URLLoaderFactory> pending;
    new IpfsURLLoaderFactory(from + "/" + scheme,
                             pending.InitWithNewPipeAndPassReceiver(), context,
                             default_factory);
    in_out->emplace(scheme, std::move(pending));
  }
}

ipfs::IpfsURLLoaderFactory::IpfsURLLoaderFactory(
    std::string dn,
    mojo::PendingReceiver<network::mojom::URLLoaderFactory> factory_receiver,
    content::BrowserContext* context,
    URLLoaderFactory* default_factory)
    : network::SelfDeletingURLLoaderFactory(std::move(factory_receiver)),
      debugging_name_{dn},
      context_{context},
      default_factory_{default_factory} {}

ipfs::IpfsURLLoaderFactory::~IpfsURLLoaderFactory() noexcept {}

void ipfs::IpfsURLLoaderFactory::CreateLoaderAndStart(
    mojo::PendingReceiver<network::mojom::URLLoader> loader,
    int32_t /*request_id*/,
    uint32_t /*options*/,
    network::ResourceRequest const& request,
    mojo::PendingRemote<network::mojom::URLLoaderClient> client,
    net::MutableNetworkTrafficAnnotationTag const&  // traffic_annotation
) {
  std::clog << "\n\n\t###\t" << __PRETTY_FUNCTION__
            << " name=" << debugging_name_ << " url=" << request.url.spec()
            << "\t###\n"
            << std::endl;
  auto ptr = std::make_shared<Loader>(
      default_factory_, InterRequestState::FromBrowserContext(context_));
  ptr->StartRequest(ptr, request, std::move(loader), std::move(client));
}