#ifndef IPFS_CHROMIUM_NULL_DNS_TXT_LOOKUP_H
#define IPFS_CHROMIUM_NULL_DNS_TXT_LOOKUP_H

#include "dns_txt_lookup.h"

namespace ipfs::ctx {
class NullDnsTxtLookup : public DnsTxtLookup {
  void SendDnsTextRequest(std::string,
                          DnsTextResultsCallback,
                          DnsTextCompleteCallback) override;
};
}  // namespace ipfs::ctx

#endif  // IPFS_CHROMIUM_NULL_DNS_TXT_LOOKUP_H
