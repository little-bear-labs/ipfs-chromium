#ifndef IPFS_IPNS_REDIRECT_H_
#define IPFS_IPNS_REDIRECT_H_

#include "ipfs_url_loader.h"

#include <ipfs_client/name_listener.h>
#include <vocab/raw_ptr.h>

#include <mojo/public/cpp/bindings/receiver.h>
#include <services/network/public/cpp/resolve_host_client_base.h>
#include <services/network/public/mojom/url_loader.mojom.h>
#include <url/gurl.h>

#include <optional>
#include <string>

namespace network {
struct ResourceRequest;
namespace mojom {
class NetworkContext;
class URLLoader;
class URLLoaderClient;
}  // namespace mojom
}  // namespace network

namespace ipfs {
class CacheRequestor;
class ContextApi;
class InterRequestState;

class IpnsUrlLoader : public network::ResolveHostClientBase,
                      public network::mojom::URLLoader,
                      public NameListener {
  raw_ref<InterRequestState> state_;
  std::string host_;
  std::optional<network::ResourceRequest> request_;
  mojo::Receiver<network::mojom::ResolveHostClient> recv_{this};
  mojo::PendingReceiver<network::mojom::URLLoader> loader_receiver_;
  mojo::PendingRemote<network::mojom::URLLoaderClient> client_remote_;
  std::shared_ptr<IpfsUrlLoader> ipfs_loader_;
  raw_ptr<network::mojom::NetworkContext> network_context_;
  std::shared_ptr<ChromiumIpfsContext> api_;
  raw_ref<network::mojom::URLLoaderFactory> http_loader_;

 public:
  explicit IpnsUrlLoader(InterRequestState& state,
                         std::string host,
                         network::mojom::NetworkContext* network_context,
                         network::mojom::URLLoaderFactory& handles_http);
  ~IpnsUrlLoader() noexcept override;

  static void StartHandling(
      std::shared_ptr<IpnsUrlLoader>,
      network::ResourceRequest const&,
      mojo::PendingReceiver<network::mojom::URLLoader>,
      mojo::PendingRemote<network::mojom::URLLoaderClient>);

 private:
  using Endpoints = std::vector<::net::HostResolverEndpointResult>;
  void OnTextResults(std::vector<std::string> const&) override;
  void OnComplete(int32_t result,
                  ::net::ResolveErrorInfo const&,
                  absl::optional<::net::AddressList> const&,
                  absl::optional<Endpoints> const&) override;

  void Next();
  void QueryDns(std::string_view);
  void DoIpfs();
  void FailNameResolution();
  network::mojom::URLLoader& under();
  bool RequestIpnsRecord();
  void RequestFromGateway();
  void CacheHit(std::shared_ptr<CacheRequestor>,std::string_view);

  void FollowRedirect(
      std::vector<std::string> const& removed_headers,
      ::net::HttpRequestHeaders const& modified_headers,
      ::net::HttpRequestHeaders const& modified_cors_exempt_headers,
      absl::optional<::GURL> const& new_url) override;
  void SetPriority(::net::RequestPriority priority,
                   int32_t intra_priority_value) override;
  void PauseReadingBodyFromNet() override;
  void ResumeReadingBodyFromNet() override;

  void Complete() override;  // From NameListener
};
}  // namespace ipfs

#endif  // IPFS_IPNS_REDIRECT_H_
