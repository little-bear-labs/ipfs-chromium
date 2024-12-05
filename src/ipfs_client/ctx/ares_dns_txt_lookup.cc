#include <ipfs_client/ctx/ares_dns_txt_lookup.h>

#if __has_include(<boost/asio/io_context.hpp>)
#include <boost/asio/io_context.hpp>
#define HAS_IO_CONTEXT 1
#endif

#if __has_include(<arpa/nameser.h>)
#include <arpa/nameser.h>
#endif

#if HAS_ARES

#include <ares.h>
#include <ares_nameser.h>

#include "log_macros.h"

using Self = ipfs::ctx::AresDnsTxtLookup;

namespace {
struct CallbackCallback {
  Self* me;
  std::string host;
};
}  // namespace
extern "C" {
static void c_ares_c_callback(void* cbcb_voidptr,
                              int status,
                              int /*timeouts*/,
                              unsigned char* abuf,
                              int alen) {
  auto cbcb = reinterpret_cast<CallbackCallback*>(cbcb_voidptr);
  struct ares_txt_reply* txt_out = nullptr;
  LOG(INFO) << "Buffer contains " << alen << " bytes.";
  if ((abuf != nullptr) && (alen != 0) && (ares_parse_txt_reply(abuf, alen, &txt_out) == 0) && (txt_out != nullptr)) {
    cbcb->me->DnsResults(cbcb->host, txt_out);
    ares_free_data(txt_out);
  } else {
    cbcb->me->DnsResults(cbcb->host, nullptr);
    LOG(ERROR) << "c_ares status=" << status;
  }
  delete cbcb;
}
}

Self::AresDnsTxtLookup(boost::asio::io_context* io) : io_{io} {
  if (ares_library_init(ARES_LIB_INIT_ALL) != 0) {
    throw std::runtime_error("Failed to initialize c-ares library.");
  }
  if (ares_init(&ares_channel_) != 0) {
    throw std::runtime_error("Failed to initialize c-ares channel.");
  }
}
Self::~AresDnsTxtLookup() noexcept {
  pending_dns_.clear();
  ares_destroy(ares_channel_);
  ares_library_cleanup();
}

void Self::SendDnsTextRequest(
    std::string host,
    ipfs::ctx::DnsTxtLookup::DnsTextResultsCallback rcb,
    ipfs::ctx::DnsTxtLookup::DnsTextCompleteCallback ccb) {
  auto p = pending_dns_.emplace(host, std::vector<DnsCbs>{});
  auto it = p.first;
  auto is_first = p.second;
  it->second.emplace_back(DnsCbs{rcb, ccb});
  if (is_first) {
    auto cbcb = new CallbackCallback;
    cbcb->me = this;
    cbcb->host = host;
    ares_query(ares_channel_, it->first.c_str(), ns_c_in, ns_t_txt,
               &c_ares_c_callback, cbcb);
#if HAS_IO_CONTEXT
    if (io_ != nullptr) {
      io_->post([this]() { CAresProcess(); });
    } else
#endif
    {
      CAresProcess();
    }
  }
}

void Self::DnsResults(std::string& host, ares_txt_reply const* result) {
  auto i = pending_dns_.find(host);
  if (pending_dns_.end() == i) {
    return;
  }
  std::vector<std::string> v{std::string{}};
  for (auto r = result; r != nullptr; r = r->next) {
    auto p = reinterpret_cast<char const*>(r->txt);
    v[0].assign(p, r->length);
    for (auto& cbs : i->second) {
      cbs.r(v);
    }
  }
  for (auto& cbs : i->second) {
    cbs.c();
  }
  pending_dns_.erase(i);
}
void Self::CAresProcess() {
  fd_set readers;
  fd_set writers;
  struct timeval tv;
  struct timeval *tvp;
  FD_ZERO(&readers);
  FD_ZERO(&writers);
  auto nfds = ares_fds(ares_channel_, &readers, &writers);
  if (nfds != 0) {
    tv.tv_sec = 30;
    tvp = ares_timeout(ares_channel_, &tv, &tv);
    auto count = select(nfds, &readers, &writers, nullptr, tvp);
    ares_process(ares_channel_, &readers, &writers);
    nfds += count;
  }
  if ((nfds != 0) || (static_cast<unsigned int>(!pending_dns_.empty()) != 0U)) {
#if HAS_IO_CONTEXT
    if (io_ != nullptr) {
      io_->post([this]() { CAresProcess(); });
    } else
#endif
    {
      CAresProcess();
    }
  }
}

#endif  // HAS_ARES
