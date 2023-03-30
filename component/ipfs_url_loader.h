#ifndef COMPONENTS_IPFS_URL_LOADER_H_
#define COMPONENTS_IPFS_URL_LOADER_H_ 1

#include "ipfs_client/dag_listener.h"
#include "ipfs_client/scheduler.h"

#include "base/debug/debugging_buildflags.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/system/data_pipe.h"
#include "services/network/public/cpp/resolve_host_client_base.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/mojom/url_loader.mojom.h"

#include <list>

namespace ipfs {
class GatewayRequests;
class UnixFsPathResolver;
}  // namespace ipfs

namespace network::mojom {
class URLLoaderFactory;
class HostResolver;
class NetworkContext;
}  // namespace network::mojom
namespace network {
class SimpleURLLoader;
}

namespace ipfs {
class InterRequestState;

class IpfsUrlLoader final : public network::mojom::URLLoader,
                            public DagListener {
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
  explicit IpfsUrlLoader(network::mojom::URLLoaderFactory& handles_http,
                         InterRequestState& state);
  ~IpfsUrlLoader() noexcept override;

  using ptr = std::shared_ptr<IpfsUrlLoader>;

  // Passed as the RequestHandler for
  // Interceptor::MaybeCreateLoader.
  static void StartRequest(
      ptr,
      network::ResourceRequest const& resource_request,
      mojo::PendingReceiver<network::mojom::URLLoader> receiver,
      mojo::PendingRemote<network::mojom::URLLoaderClient> client);

  void OverrideUrl(GURL);
  void extra(std::shared_ptr<network::mojom::URLLoader> xtra) { extra_ = xtra; }

 private:
  using RequestHandle = std::unique_ptr<network::SimpleURLLoader>;

  InterRequestState& state_;
  mojo::Receiver<network::mojom::URLLoader> receiver_{this};
  mojo::Remote<network::mojom::URLLoaderClient> client_;
  network::mojom::URLLoaderFactory& lower_loader_factory_;
  mojo::ScopedDataPipeProducerHandle pipe_prod_ = {};
  mojo::ScopedDataPipeConsumerHandle pipe_cons_ = {};
  bool complete_ = false;
  std::shared_ptr<GatewayRequests> api_;
  std::string original_url_;
  std::shared_ptr<ipfs::UnixFsPathResolver> resolver_;
  std::string partial_block_;
  std::shared_ptr<network::mojom::URLLoader> extra_;

  void CreateBlockRequest(std::string cid);

  void ReceiveBlockBytes(std::string_view) override;
  void BlocksComplete(std::string mime_type) override;
  void FourOhFour(std::string_view cid, std::string_view path) override;

  void StartUnixFsProc(ptr, std::string_view);
};

}  // namespace ipfs

#endif
