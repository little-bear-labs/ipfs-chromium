#ifndef COMPONENTS_IPFS_URL_LOADER_H_
#define COMPONENTS_IPFS_URL_LOADER_H_ 1

#include "ipfs_client/scheduler.h"

#include "base/debug/debugging_buildflags.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/system/data_pipe.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/mojom/url_loader.mojom.h"

#include <list>

namespace ipfs {
class UnixFsPathResolver;
}

namespace network::mojom {
class URLLoaderFactory;
}
namespace network {
class SimpleURLLoader;
}

namespace ipfs {
class InterRequestState;

class Loader final : public network::mojom::URLLoader,
                     public std::enable_shared_from_this<Loader> {
  void FollowRedirect(
      std::vector<std::string> const& removed_headers,
      net::HttpRequestHeaders const& modified_headers,
      net::HttpRequestHeaders const& modified_cors_exempt_headers,
      absl::optional<::GURL> const& new_url) override;
  void SetPriority(net::RequestPriority priority,
                   int32_t intra_priority_value) override;
  void PauseReadingBodyFromNet() override;
  void ResumeReadingBodyFromNet() override;

 public:
  explicit Loader(network::mojom::URLLoaderFactory* handles_http,
                  InterRequestState& state);
  ~Loader() noexcept override;

  using ptr = std::shared_ptr<Loader>;

  // Passed as the RequestHandler for
  // Interceptor::MaybeCreateLoader.
  static void StartRequest(
      ptr,
      network::ResourceRequest const& resource_request,
      mojo::PendingReceiver<network::mojom::URLLoader> receiver,
      mojo::PendingRemote<network::mojom::URLLoaderClient> client);

 private:
  using RequestHandle = std::unique_ptr<network::SimpleURLLoader>;

  InterRequestState& state_;
  mojo::Receiver<network::mojom::URLLoader> receiver_{this};
  mojo::Remote<network::mojom::URLLoaderClient> client_;
  network::mojom::URLLoaderFactory* lower_loader_factory_;
  mojo::ScopedDataPipeProducerHandle pipe_prod_ = {};
  mojo::ScopedDataPipeConsumerHandle pipe_cons_ = {};
  bool complete_ = false;
  Scheduler sched_;
  std::list<std::pair<BusyGateway, RequestHandle>> gateway_requests_;
  std::string original_url_;
  std::shared_ptr<ipfs::UnixFsPathResolver> resolver_;
  std::string partial_block_;

  void startup(ptr,
               std::string requested_path,
               unsigned concurrent_requests = 10);
  bool start_gateway_request(ptr, std::string requested_path);
  bool start_gateway_request(ptr,
                             GatewayList& free_gws,
                             GatewayList& busy_gws,
                             std::string requested_path);
  void OnGatewayResponse(ptr,
                         std::string requested_path,
                         std::size_t,
                         std::unique_ptr<std::string>);
  bool handle_response(Gateway& gw,
                       network::SimpleURLLoader* gw_req,
                       std::string* body);
  void CreateRequest(BusyGateway&&);
  void CreateBlockRequest(std::string cid);
  void BlocksComplete(std::string mime_type);
  bool HandleBlockResponse(Gateway&,
                           std::string const&,
                           network::mojom::URLResponseHead const&);
};

}  // namespace ipfs

#endif
