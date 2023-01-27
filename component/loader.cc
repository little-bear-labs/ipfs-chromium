#include "loader.h"

using Self = ipfs::Loader;

Self::Loader()
{
  std::clog << "\n\n ### HIT " << __PRETTY_FUNCTION__ << " ### \n\n";
}

void Self::FollowRedirect(
      std::vector<std::string> const& removed_headers
    , net::HttpRequestHeaders  const& modified_headers
    , net::HttpRequestHeaders  const& modified_cors_exempt_headers
    , absl::optional<::GURL>   const& new_url
    )
{
  std::clog << "\n\n ### HIT " << __PRETTY_FUNCTION__ << " ### \n\n";
}
void Self::SetPriority(net::RequestPriority priority, int32_t intra_priority_value)
{
  std::clog << "\n\n ### HIT " << __PRETTY_FUNCTION__ << " ### \n\n";
}
void Self::PauseReadingBodyFromNet()
{
  std::clog << "\n\n ### HIT " << __PRETTY_FUNCTION__ << " ### \n\n";
}
void Self::ResumeReadingBodyFromNet()
{
  std::clog << "\n\n ### HIT " << __PRETTY_FUNCTION__ << " ### \n\n";
}

void Self::StartRequest(
    network::ResourceRequest const& resource_request
  , mojo::PendingReceiver<network::mojom::URLLoader      > receiver
  , mojo::PendingRemote  <network::mojom::URLLoaderClient> client
  )
{
  std::clog << "\n\n ### HIT " << __PRETTY_FUNCTION__ << " ### \n\n";
}
