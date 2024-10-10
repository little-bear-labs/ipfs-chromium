#include <ipfs_client/ctx/null_dns_txt_lookup.h>
#include <string>

#include "log_macros.h"

using Self = ipfs::ctx::NullDnsTxtLookup;

void Self::SendDnsTextRequest(std::string /*hostname*/,
                              DnsTextResultsCallback /*unused*/,
                              DnsTextCompleteCallback callback) {
  LOG(ERROR)
      << "DNS TXT lookup provider not instantiated. DNSLink will not work.";
  callback();
}
