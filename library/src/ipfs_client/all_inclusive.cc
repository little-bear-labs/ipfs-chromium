#include <ipfs_client/all_inclusive.h>

#if HAS_ALL_INCLUSIVE

#if __has_include(<arpa/nameser.h>)
#include <arpa/nameser.h>
#endif

#include <ares_nameser.h>

#include "log_macros.h"

using Self = ipfs::AllInclusiveContext;

namespace {
struct CallbackCallback {
  Self* me;
  std::shared_ptr<ipfs::ContextApi> alsome;
  std::string host;
};
}  // namespace
extern "C" {
static void c_ares_c_callback(void* vp,
                              int status,
                              int /*timeouts*/,
                              unsigned char* abuf,
                              int alen) {
  auto cbcb = reinterpret_cast<CallbackCallback*>(vp);
  struct ares_txt_reply* txt_out = nullptr;
  LOG(INFO) << "Buffer contains " << alen << " bytes.";
  if (abuf && alen && !ares_parse_txt_reply(abuf, alen, &txt_out) && txt_out) {
    cbcb->me->DnsResults(cbcb->host, *txt_out);
    ares_free_data(txt_out);
  } else {
    LOG(ERROR) << "c_ares status=" << status;
  }
  delete cbcb;
}
}

Self::AllInclusiveContext(boost::asio::io_context& io) : io_{io} {
  if (ares_library_init(ARES_LIB_INIT_ALL)) {
    throw std::runtime_error("Failed to initialize c-ares library.");
  }
  if (ares_init(&ares_channel_)) {
    throw std::runtime_error("Failed to initialize c-ares channel.");
  }
}
Self::~AllInclusiveContext() {
  pending_dns_.clear();
  ares_destroy(ares_channel_);
  ares_library_cleanup();
}
void Self::SendDnsTextRequest(std::string host,
                              DnsTextResultsCallback rcb,
                              DnsTextCompleteCallback ccb) {
  auto p = pending_dns_.emplace(host, std::vector<DnsCbs>{});
  auto it = p.first;
  auto is_first = p.second;
  it->second.emplace_back(DnsCbs{rcb, ccb});
  LOG(INFO) << __PRETTY_FUNCTION__ << ' ' << host << ' ' << is_first;
  if (is_first) {
    auto cbcb = new CallbackCallback;
    cbcb->me = this;
    cbcb->alsome = shared_from_this();
    cbcb->host = host;
    ares_query(ares_channel_, it->first.c_str(), ns_c_in, ns_t_txt,
               &c_ares_c_callback, cbcb);
    io_.post([this]() { CAresProcess(); });
  }
}
void Self::DnsResults(std::string& host, ares_txt_reply& result) {
  LOG(INFO) << __PRETTY_FUNCTION__ << ' ' << host;
  auto i = pending_dns_.find(host);
  if (pending_dns_.end() == i) {
    return;
  }
  std::vector<std::string> v{std::string{}};
  for (auto r = &result; r; r = r->next) {
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
  LOG(INFO) << __PRETTY_FUNCTION__ << ' ' << pending_dns_.size();
  fd_set readers, writers;
  struct timeval tv, *tvp;
  FD_ZERO(&readers);
  FD_ZERO(&writers);
  auto nfds = ares_fds(ares_channel_, &readers, &writers);
  if (nfds) {
    tvp = ares_timeout(ares_channel_, NULL, &tv);
    auto count = select(nfds, &readers, &writers, NULL, tvp);
    ares_process(ares_channel_, &readers, &writers);
    nfds += count;
  }
  if (nfds || pending_dns_.size()) {
    io_.post([this]() { CAresProcess(); });
  }
}
void Self::SendHttpRequest(HttpRequestDescription desc,
                           HttpCompleteCallback cb) const {
  auto sess = std::make_shared<HttpSession>(io_, desc, cb);
  sess->run();
}

#endif
