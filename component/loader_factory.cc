#include "loader_factory.h"
#include <iostream>

using Factory = ipfs::LoaderFactory;

Factory::LoaderFactory(mojo::PendingReceiver<network::mojom::URLLoaderFactory>&& receiver)
: network::SelfDeletingURLLoaderFactory{std::move(receiver)}
{
  std::clog << __PRETTY_FUNCTION__ << std::endl;
}

void Factory::CreateLoaderAndStart(
    mojo::PendingReceiver<network::mojom::URLLoader> receiver
    , int32_t request_id
    , uint32_t options
    , network::ResourceRequest const& resource_request
    , mojo::PendingRemote<network::mojom::URLLoaderClient> client
    , net::MutableNetworkTrafficAnnotationTag const& traffic_annotation
    )
{
  std::clog << __PRETTY_FUNCTION__ << std::endl;
  std::abort();
}

mojo::PendingRemote<network::mojom::URLLoaderFactory> Factory::Create(
      int frame_tree_node_id
    , content::BrowserContext* browser_context
    , ipfs::Scheme
    )
{
  std::clog << __PRETTY_FUNCTION__ << std::endl;
  return {};
}
