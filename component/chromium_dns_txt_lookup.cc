#include "chromium_dns_txt_lookup.h"

#include "inter_request_state.h"

using Self = ipfs::ChromiumDnsTxtLookup;

void Self::SendDnsTextRequest(std::string host,
                              DnsTextResultsCallback res,
                              DnsTextCompleteCallback don) {
  if (dns_reqs_.find(host) != dns_reqs_.end()) {
    // TODO: do combine two or delay this one. Don't drop the first one!!
    VLOG(2) << "Requested resolution of DNSLink host " << host
            << " multiple times.";
  }
  auto don_wrap = [don, this, host]() {
    don();
    VLOG(2) << "Finished resolving " << host << " via DNSLink";
    dns_reqs_.erase(host);
  };
  auto* nc = state_->network_context();
  dns_reqs_[host] = std::make_unique<DnsTxtRequest>(host, res, don_wrap, nc);
}
Self::ChromiumDnsTxtLookup(InterRequestState& st) : state_{&st} {}
Self::~ChromiumDnsTxtLookup() {}