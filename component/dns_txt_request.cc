#include "dns_txt_request.h"

#include <services/network/network_context.h>
#include <services/network/public/mojom/host_resolver.mojom.h>

namespace moj = network::mojom;
using Self = ipfs::DnsTxtRequest;

Self::DnsTxtRequest(std::string host,
                    ipfs::ContextApi::DnsTextResultsCallback res,
                    ipfs::ContextApi::DnsTextCompleteCallback don,
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
  LOG(INFO) << "Hit " << results.size() << " DNS TXT results.";
  results_callback_(results);
}
void Self::OnComplete(int32_t result,
                      const ::net::ResolveErrorInfo&,
                      const absl::optional<::net::AddressList>&,
                      const absl::optional<Endpoints>&) {
  VLOG(1) << "DNS Results done with code: " << result;
  completion_callback_();
}
