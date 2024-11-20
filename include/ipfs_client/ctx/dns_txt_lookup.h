#ifndef IPFS_CHROMIUM_DNS_TXT_LOOKUP_H
#define IPFS_CHROMIUM_DNS_TXT_LOOKUP_H

#include <functional>
#include <string>

namespace ipfs::ctx {
/*! Handles requesting TXT (text) request from a DNS system
 *  @note Subclassed by implementation.
 */
class DnsTxtLookup {
 public:
  /*! Callback for if usable result was achieved
   */
  using DnsTextResultsCallback =
      std::function<void(std::vector<std::string> const&)>;
  using DnsTextCompleteCallback = std::function<void(void)>;
  virtual void SendDnsTextRequest(std::string hostname,
                                  DnsTextResultsCallback,
                                  DnsTextCompleteCallback) = 0;

  virtual ~DnsTxtLookup() noexcept;
};
}  // namespace ipfs::ctx

#endif  // IPFS_CHROMIUM_DNS_TXT_LOOKUP_H
