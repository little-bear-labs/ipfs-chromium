#ifndef IPFS_CHROMIUM_CHROMIUM_DNS_TXT_LOOKUP_H
#define IPFS_CHROMIUM_CHROMIUM_DNS_TXT_LOOKUP_H

#include "dns_txt_request.h"

#include <ipfs_client/ctx/dns_txt_lookup.h>

namespace ipfs {
class InterRequestState;
/*! Lookup TXT records from a DNS service using Chromium facilities
 *  @note This is currently fragile, as only DNSClient is capable of these requests
 *    yet DNSClient may not always be available, depending on Chromium's parsing of
 *    system config files.
 *  @todo Fix the note.
 */
class ChromiumDnsTxtLookup : public ctx::DnsTxtLookup {
  raw_ptr<InterRequestState> state_;
  std::map<std::string, std::vector<std::unique_ptr<DnsTxtRequest>>> dns_reqs_;

  void SendDnsTextRequest(std::string host,
                          DnsTextResultsCallback,
                          DnsTextCompleteCallback) override;

 public:

  /*!
   * \brief construct
   * \param state Access to nigh-globals, in particular network context.
   */
  ChromiumDnsTxtLookup(InterRequestState& state);

  /*!
   * \brief dtor
   */
  ~ChromiumDnsTxtLookup() noexcept override;
};
}  // namespace ipfs

#endif  // IPFS_CHROMIUM_CHROMIUM_DNS_TXT_LOOKUP_H
