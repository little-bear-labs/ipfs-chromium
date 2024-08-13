#ifndef IPFS_CHROMIUM_ARES_DNS_TXT_LOOKUP_H
#define IPFS_CHROMIUM_ARES_DNS_TXT_LOOKUP_H

#include "dns_txt_lookup.h"

#include <map>
#include <stdexcept>

#if __has_include(<ares.h>)
#include <ares.h>
#define HAS_ARES 1

namespace boost::asio {
class io_context;
}
namespace ipfs::ctx {
class AresDnsTxtLookup : public DnsTxtLookup {
  struct DnsCbs {
    DnsTextResultsCallback r;
    DnsTextCompleteCallback c;
  };
  std::map<std::string, std::vector<DnsCbs>> pending_dns_;
  boost::asio::io_context* io_;
  ares_channel_t* ares_channel_ = nullptr;
  void CAresProcess();

 public:
  AresDnsTxtLookup(boost::asio::io_context* io = nullptr);
  ~AresDnsTxtLookup() noexcept override;
  void SendDnsTextRequest(std::string,
                          DnsTextResultsCallback,
                          DnsTextCompleteCallback) override;
  void DnsResults(std::string&, ares_txt_reply&);
};
}  // namespace ipfs::ctx

#endif  // HAS_ARES
#endif  // IPFS_CHROMIUM_ARES_DNS_TXT_LOOKUP_H
