#ifndef IPFS_CHROMIUM_CHROMIUM_DNS_TXT_LOOKUP_H
#define IPFS_CHROMIUM_CHROMIUM_DNS_TXT_LOOKUP_H

#include "dns_txt_request.h"

#include <ipfs_client/ctx/dns_txt_lookup.h>

namespace ipfs {
class InterRequestState;
class ChromiumDnsTxtLookup : public ctx::DnsTxtLookup {
  raw_ptr<InterRequestState> state_;
  std::map<std::string, std::vector<std::unique_ptr<DnsTxtRequest>>> dns_reqs_;

  void SendDnsTextRequest(std::string,
                          DnsTextResultsCallback,
                          DnsTextCompleteCallback) override;

 public:
  ChromiumDnsTxtLookup(InterRequestState&);
  ~ChromiumDnsTxtLookup() noexcept override;
};
}  // namespace ipfs

#endif  // IPFS_CHROMIUM_CHROMIUM_DNS_TXT_LOOKUP_H
