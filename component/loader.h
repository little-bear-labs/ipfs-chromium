#ifndef COMPONENTS_IPFS_URL_LOADER_H_
#define COMPONENTS_IPFS_URL_LOADER_H_ 1

#include "ipfs_client/gateways.h"

#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/mojom/url_loader.mojom.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/system/data_pipe.h"

namespace network::mojom {
  class URLLoaderFactory;
}
namespace network {
  class SimpleURLLoader;
}

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
    explicit Loader(network::mojom::URLLoaderFactory* handles_http);
    ~Loader() override;

    using ptr = std::shared_ptr<Loader>;

    // Passed as the RequestHandler for
    // Interceptor::MaybeCreateLoader.
    static void StartRequest(
        ptr
      , network::ResourceRequest const& resource_request
      , mojo::PendingReceiver<network::mojom::URLLoader> receiver
      , mojo::PendingRemote<network::mojom::URLLoaderClient> client
      );

  private:
      constexpr static std::uint8_t GOOD = 3;
      constexpr static std::uint8_t MAYB = 0;
      constexpr static std::uint8_t BUSY = 1;
      constexpr static std::uint8_t FREE = 0;

      mojo::Receiver<network::mojom::URLLoader> receiver_{this};
      mojo::Remote<network::mojom::URLLoaderClient> client_;
      std::array<GatewayList,4> gateways_;
      std::vector<std::pair<std::string,std::unique_ptr<network::SimpleURLLoader>>> gateway_requests_;
      network::mojom::URLLoaderFactory* lower_loader_factory_;
      mojo::ScopedDataPipeProducerHandle pipe_prod_ = {};
      mojo::ScopedDataPipeConsumerHandle pipe_cons_ = {};
      std::string requested_path_;

      void startup(ptr, unsigned concurrent_requests = 10);
      bool start_gateway_request(ptr);
      bool start_gateway_request( ptr, GatewayList& free_gws, GatewayList& busy_gws );
      void on_gateway_response(ptr, std::size_t, std::unique_ptr<std::string>);
  };

}

#endif
