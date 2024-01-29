#include <ipfs_client/ctx/null_dns_txt_lookup.h>

#include "log_macros.h"

using Self = ipfs::ctx::NullDnsTxtLookup;

void Self::SendDnsTextRequest(std::string,
                              DnsTextResultsCallback,
                              DnsTextCompleteCallback) {
  LOG(ERROR)
      << "DNS TXT lookup provider not instantiated. DNSLink will not work.";
}