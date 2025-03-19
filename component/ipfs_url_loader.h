#ifndef COMPONENTS_IPFS_URL_LOADER_H_
#define COMPONENTS_IPFS_URL_LOADER_H_ 1

#include "virtual_optional.h"

#include "base/debug/debugging_buildflags.h"
#include "base/timer/timer.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/system/data_pipe.h"
#include "net/http/http_request_headers.h"
#include "services/network/public/cpp/resolve_host_client_base.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/mojom/url_loader.mojom.h"

#include <list>

namespace ipfs {
class Client;
class IpfsRequest;
namespace ipld {
class DagHeaders;
}
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

/*!
 * \brief A URL Loader for the ipfs:// (and ipns://) scheme.
 */
class IpfsUrlLoader final : public network::mojom::URLLoader {
  void FollowRedirect(
      std::vector<std::string> const& removed_headers,
      net::HttpRequestHeaders const& modified_headers,
      net::HttpRequestHeaders const& modified_cors_exempt_headers,
      VirtualOptional<::GURL> const& new_url) override;
  void SetPriority(net::RequestPriority priority,
                   int32_t intra_priority_value) override;

 public:
  /*!
   * \brief Construct with the needed tools
   * \param handles_http How to construct URL Loaders for HTTP
   * \param state Access to orchestrator, api, etc.
   */
  explicit IpfsUrlLoader(network::mojom::URLLoaderFactory& handles_http,
                         InterRequestState& state);
  ~IpfsUrlLoader() noexcept override;

  /*! Reference type for this URL Loader (defining memory model)
   */
  using ptr = std::shared_ptr<IpfsUrlLoader>;

  // Passed as the RequestHandler for
  // Interceptor::MaybeCreateLoader.
  static void StartRequest(
      ptr,
      network::ResourceRequest const& resource_request,
      mojo::PendingReceiver<network::mojom::URLLoader> receiver,
      mojo::PendingRemote<network::mojom::URLLoaderClient> client);

  void OverrideUrl(GURL);
  void AddHeader(std::string_view,std::string_view);
  void extra(std::shared_ptr<network::mojom::URLLoader> xtra) { extra_ = xtra; }

 private:
  using RequestHandle = std::unique_ptr<network::SimpleURLLoader>;

  raw_ref<InterRequestState> state_;
  mojo::Receiver<network::mojom::URLLoader> receiver_{this};
  mojo::Remote<network::mojom::URLLoaderClient> client_;
  raw_ref<network::mojom::URLLoaderFactory> lower_loader_factory_;
  mojo::ScopedDataPipeProducerHandle pipe_prod_ = {};
  mojo::ScopedDataPipeConsumerHandle pipe_cons_ = {};
  bool complete_ = false;
  std::shared_ptr<Client> api_;
  std::string original_url_;
  std::string partial_block_;
  std::shared_ptr<network::mojom::URLLoader> extra_;
  std::unique_ptr<base::RepeatingTimer> stepper_;
  std::string root_;
  int status_ = 200;
  std::string resp_loc_;
  std::shared_ptr<IpfsRequest> ipfs_request_;

  void ReceiveBlockBytes(std::string_view);
  void BlocksComplete(std::string mime_type, ipld::DagHeaders const&);
  void DoesNotExist(std::string_view cid, std::string_view path);
  void TakeStep();
};

}  // namespace ipfs

#endif
