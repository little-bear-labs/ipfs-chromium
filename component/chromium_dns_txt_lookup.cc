#include "chromium_dns_txt_lookup.h"

#include "inter_request_state.h"

using Self = ipfs::ChromiumDnsTxtLookup;

void Self::SendDnsTextRequest(std::string host,
                              DnsTextResultsCallback res,
                              DnsTextCompleteCallback don) {
  auto don_wrap = [don, this, host]() {
    don();
    dns_reqs_.erase(host);
  };
  auto* nc = state_->network_context();
  dns_reqs_[host].push_back(
      std::make_unique<DnsTxtRequest>(host, res, don_wrap, nc));
}
Self::ChromiumDnsTxtLookup(InterRequestState& st) : state_{&st} {}
Self::~ChromiumDnsTxtLookup() {}