#ifndef COMPONENTS_IPFS_URL_LOADER_H_
#define COMPONENTS_IPFS_URL_LOADER_H_ 1

#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/mojom/url_loader.mojom.h"

namespace ipfs {

  class Loader final : public network::mojom::URLLoader {
    void FollowRedirect(
        std::vector<std::string> const& removed_headers
      , net::HttpRequestHeaders  const& modified_headers
      , net::HttpRequestHeaders  const& modified_cors_exempt_headers
      , absl::optional<::GURL>   const& new_url
      ) override;
    void SetPriority(net::RequestPriority priority, int32_t intra_priority_value) override;
    void PauseReadingBodyFromNet() override;
    void ResumeReadingBodyFromNet() override;

  public:
    Loader();

    // Passed as the RequestHandler for
    // Interceptor::MaybeCreateLoader.
    void StartRequest(
        network::ResourceRequest const& resource_request
      , mojo::PendingReceiver<network::mojom::URLLoader      > receiver
      , mojo::PendingRemote  <network::mojom::URLLoaderClient> client
      );
  };

}

#endif
