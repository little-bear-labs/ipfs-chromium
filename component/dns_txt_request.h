#ifndef IPFS_DNS_TXT_REQUEST_H_
#define IPFS_DNS_TXT_REQUEST_H_

#include "virtual_optional.h"

#include <ipfs_client/client.h>

#include <base/check_version_internal.h>
#include <mojo/public/cpp/bindings/receiver.h>
#include <services/network/public/cpp/resolve_host_client_base.h>

namespace network::mojom {
class NetworkContext;
}

#pragma clang diagnostic ignored "-Winconsistent-missing-override"

namespace ipfs {

/*!
 * \brief A "text" (TXT) request to be sent to a DNS server
 */
class DnsTxtRequest final : public network::ResolveHostClientBase {
 public:
  using DnsTextResultsCallback = ctx::DnsTxtLookup::DnsTextResultsCallback; ///< Report results
  using DnsTextCompleteCallback = ctx::DnsTxtLookup::DnsTextCompleteCallback;

 private:
  DnsTextResultsCallback results_callback_;
  DnsTextCompleteCallback completion_callback_;
  mojo::Receiver<network::mojom::ResolveHostClient> recv_{this};

  using Endpoints = std::vector<::net::HostResolverEndpointResult>;
  void OnTextResults(std::vector<std::string> const&) override;

  void OnComplete(int32_t result,
                  ::net::ResolveErrorInfo const&,
                  ::net::AddressList const&,
                  std::vector<::net::HostResolverEndpointResult> const&)
    #if BASE_CHECK_VERSION_INTERNAL > 139
        override
    #endif
    ;
  void OnComplete(int32_t result,
                  ::net::ResolveErrorInfo const&,
                  VirtualOptional<::net::AddressList> const&,
                  VirtualOptional<Endpoints> const&)
    #if BASE_CHECK_VERSION_INTERNAL < 140
        override
    #endif
    ;

 public:

  /*!
   * \brief construct a request
   * \param host The hostname to request on
   * \param res  Callback to call for results. Called 0 or more times.
   * \param don  Callback when res is finished being called. Called once.
   * \param network_context Network to use for these requests
   */
  DnsTxtRequest(std::string host,
                DnsTextResultsCallback res,
                DnsTextCompleteCallback don,
                network::mojom::NetworkContext* network_context);
  DnsTxtRequest(DnsTxtRequest&&) = delete;
  ~DnsTxtRequest() noexcept override;
};
}  // namespace ipfs

#endif  // IPFS_DNS_TXT_REQUEST_H_
