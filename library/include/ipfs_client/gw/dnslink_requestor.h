#ifndef IPFS_DNSLINK_REQUESTOR_H_
#define IPFS_DNSLINK_REQUESTOR_H_

#include "requestor.h"

namespace ipfs {
class Client;
}

namespace ipfs::gw {
/*! Link in chain-of-command for request for a DNSLink name
 */
class DnsLinkRequestor final : public Requestor {
 public:
  /*! @param api The Client context API
   */
  explicit DnsLinkRequestor(std::shared_ptr<Client> api);

  HandleOutcome handle(RequestPtr) override;
  std::string_view name() const override;
};
}  // namespace ipfs::gw

#endif  // IPFS_DNSLINK_REQUESTOR_H_
