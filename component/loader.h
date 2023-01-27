#ifndef COMPONENTS_IPFS_URL_LOADER_H_
#define COMPONENTS_IPFS_URL_LOADER_H_ 1

#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/mojom/url_loader.mojom.h"

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
    explicit Loader();
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
      mojo::Receiver<network::mojom::URLLoader> receiver_{this};
      mojo::Remote<network::mojom::URLLoaderClient> client_;
      std::vector<std::string> gateways_;
      std::size_t gateway_index_ = 0UL;
      std::vector<std::unique_ptr<network::SimpleURLLoader>> gateway_requests_;

      void startup(ptr, unsigned concurrent_requests = 10);
      bool start_gateway_request(ptr);
      void start_gateway_request(ptr,std::string& gateway);
      void on_gateway_response(ptr, std::unique_ptr<std::string>);
  };

}

#endif
