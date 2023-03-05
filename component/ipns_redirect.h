#ifndef IPFS_IPNS_REDIRECT_H_
#define IPFS_IPNS_REDIRECT_H_

#include <mojo/public/cpp/bindings/receiver.h>
#include <services/network/public/cpp/resolve_host_client_base.h>
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
class InterRequestState;
class IpnsRedirect : public network::ResolveHostClientBase {
  InterRequestState& state_;
  std::string host_;
  std::optional<GURL> requested_uri_;
  mojo::Receiver<network::mojom::ResolveHostClient> recv_{this};
  std::shared_ptr<IpnsRedirect> lifetime_extension_;
  //  mojo::PendingReceiver<network::mojom::URLLoader> loader_receiver_;
  mojo::PendingRemote<network::mojom::URLLoaderClient> client_remote_;

 public:
  explicit IpnsRedirect(InterRequestState& state,
                        std::string host,
                        network::mojom::NetworkContext* network_context);
  ~IpnsRedirect() noexcept override;

  static void DoRedirect(std::shared_ptr<IpnsRedirect>,
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
  void CreateRedirect();
};
}  // namespace ipfs

#endif  // IPFS_IPNS_REDIRECT_H_
