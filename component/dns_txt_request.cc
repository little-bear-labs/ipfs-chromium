#include "dns_txt_request.h"

#include <services/network/network_context.h>
#include <services/network/public/mojom/host_resolver.mojom.h>

namespace moj = network::mojom;
using Self = ipfs::DnsTxtRequest;

Self::DnsTxtRequest(std::string host,
                    DnsTextResultsCallback res,
                    DnsTextCompleteCallback don,
                    moj::NetworkContext* network_context)
    : results_callback_{res}, completion_callback_{don} {
  auto params = moj::ResolveHostParameters::New();
  params->dns_query_type = net::DnsQueryType::TXT;
  params->initial_priority = net::RequestPriority::HIGHEST;
  params->source = net::HostResolverSource::ANY;
  params->cache_usage = moj::ResolveHostParameters_CacheUsage::STALE_ALLOWED;
  params->secure_dns_policy = moj::SecureDnsPolicy::ALLOW;
  params->purpose = moj::ResolveHostParameters::Purpose::kUnspecified;
  auto hrh = moj::HostResolverHost::NewHostPortPair({host, 0});
  auto nak = net::NetworkAnonymizationKey::CreateTransient();
  network_context->ResolveHost(std::move(hrh), nak, std::move(params),
                               recv_.BindNewPipeAndPassRemote());
}
Self::~DnsTxtRequest() {}

void Self::OnTextResults(std::vector<std::string> const& results) {
  results_callback_(results);
}
void Self::OnComplete(int32_t result,
                      ::net::ResolveErrorInfo const&,
                      VirtualOptional<::net::AddressList> const&,
                      VirtualOptional<Endpoints> const&) {
  completion_callback_();
}
void Self::OnComplete(int32_t result,
                ::net::ResolveErrorInfo const&,
                ::net::AddressList const&,
                std::vector<::net::HostResolverEndpointResult> const&) {
  completion_callback_();
}
