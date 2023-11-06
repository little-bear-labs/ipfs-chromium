#ifndef IPFS_DNS_TXT_REQUEST_H_
#define IPFS_DNS_TXT_REQUEST_H_

#include <ipfs_client/context_api.h>

#include <mojo/public/cpp/bindings/receiver.h>
#include <services/network/public/cpp/resolve_host_client_base.h>

namespace network::mojom {
class NetworkContext;
}

namespace ipfs {
class DnsTxtRequest final : public network::ResolveHostClientBase {
  ipfs::ContextApi::DnsTextResultsCallback results_callback_;
  ipfs::ContextApi::DnsTextCompleteCallback completion_callback_;
  mojo::Receiver<network::mojom::ResolveHostClient> recv_{this};

  using Endpoints = std::vector<::net::HostResolverEndpointResult>;
  void OnTextResults(std::vector<std::string> const&) override;
  void OnComplete(int32_t result,
                  ::net::ResolveErrorInfo const&,
                  absl::optional<::net::AddressList> const&,
                  absl::optional<Endpoints> const&) override;

 public:
  DnsTxtRequest(std::string,
                ipfs::ContextApi::DnsTextResultsCallback,
                ipfs::ContextApi::DnsTextCompleteCallback,
                network::mojom::NetworkContext*);
  DnsTxtRequest(DnsTxtRequest&&) = delete;
  ~DnsTxtRequest() noexcept override;
};
}  // namespace ipfs

#endif  // IPFS_DNS_TXT_REQUEST_H_
