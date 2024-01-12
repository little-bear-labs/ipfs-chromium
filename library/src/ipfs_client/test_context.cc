#include <ipfs_client/test_context.h>

#if HAS_ALL_INCLUSIVE

#if __has_include(<arpa/nameser.h>)
#include <arpa/nameser.h>
#endif

#include <ares_nameser.h>

#include "log_macros.h"

using Self = ipfs::TestContext;

// LCOV_EXCL_START

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

Self::TestContext(boost::asio::io_context& io)
    : gateways_{Gateways::DefaultGateways()}, io_{io} {
  std::sort(gateways_.begin(), gateways_.end(),
            [](auto& a, auto& b) { return a.prefix < b.prefix; });
  if (ares_library_init(ARES_LIB_INIT_ALL)) {
    throw std::runtime_error("Failed to initialize c-ares library.");
  }
  if (ares_init(&ares_channel_)) {
    throw std::runtime_error("Failed to initialize c-ares channel.");
  }
}
Self::~TestContext() {
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
namespace http = boost::beast::http;
class HttpSession : public std::enable_shared_from_this<HttpSession> {
  using tcp = boost::asio::ip::tcp;
  boost::asio::io_context& ioc_;
  boost::asio::strand<boost::asio::io_context::executor_type> strand_;
  tcp::resolver resolver_;
  boost::asio::ssl::context& ssl_ctx_;
  boost::beast::ssl_stream<boost::beast::tcp_stream> stream_;
  boost::beast::flat_buffer buffer_;  // (Must persist between reads)
  http::request<http::empty_body> req_;
  ipfs::ContextApi::HttpCompleteCallback cb_;
  int expiry_seconds_ = 91;
  std::string host_, port_, target_;
  ipfs::HttpRequestDescription desc_;
  static std::map<std::string, tcp::resolver::results_type> resolutions_;
  std::string parsed_host_;
  http::response_parser<http::string_body> response_parser_;
  std::optional<http::response<http::string_body>> res_;
  std::shared_ptr<HttpSession> prev;

  void fail(boost::beast::error_code ec, char const* what) {
    GOOGLE_LOG(INFO) << what << ": " << ec.value() << ' ' << ec.message()
                     << " URL:" << desc_.url << " HOST:" << host_
                     << " PORT:" << port_ << " TARGET:" << target_;
    auto status = ec.value() == 1 ? 408 : 500;
    cb_(status, "", [](auto) { return std::string{}; });
  }
  std::string parse_url() {
    ipfs::SlashDelimited ss{desc_.url};
    auto scheme = ss.pop();
    if (port_.empty()) {
      port_.assign(scheme);
      if (port_.back() == ':') {
        port_.resize(port_.size() - 1UL);
      }
    }
    std::string host{ss.pop()};
    auto colon = host.find(':');
    if (colon < host.size()) {
      port_ = host.substr(colon + 1UL);
      host.resize(colon);
    } else if (port_ == "https") {
      port_ = "443";
    } else if (port_ == "http") {
      port_ = "80";
    }
    target_.assign("/").append(ss.to_string());
    return host;
  }

 public:
  explicit HttpSession(boost::asio::io_context& ioc,
                       boost::asio::ssl::context& ssc,
                       ipfs::HttpRequestDescription& desc,
                       ipfs::ContextApi::HttpCompleteCallback cb)
      : ioc_{ioc},
        strand_{boost::asio::make_strand(ioc)},
        resolver_(strand_),
        ssl_ctx_(ssc),
        stream_(strand_, ssc),
        cb_{cb},
        desc_{desc} {
    if (auto sz = desc_.max_response_size) {
      response_parser_.body_limit(*sz * 2);
    } else {
      response_parser_.body_limit(boost::none);
    }
  }
  tcp::resolver::results_type& resolution() {
    return resolutions_[host_ + port_];
  }
  // Start the asynchronous operation
  void run() {
    auto parsed_host_ = parse_url();
    if (host_.empty()) {
      host_ = parsed_host_;
    }
    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(stream_.native_handle(), host_.c_str())) {
      boost::beast::error_code ec{static_cast<int>(::ERR_get_error()),
                                  boost::asio::error::get_ssl_category()};
      GOOGLE_LOG(ERROR) << "SSL early fail: " << ec.message();
      return;
    }

    req_.version(11);
    req_.method(http::verb::get);
    req_.target(target_);
    req_.set(http::field::host, parsed_host_);
    if (desc_.accept.size()) {
      //      std::clog << "Setting Accept: " << desc_.accept << '\n';
      req_.set("Accept", desc_.accept);
    }
    extend_time();
    auto me = shared_from_this();
    if (resolution().empty()) {
      GOOGLE_LOG(DEBUG) << "Starting " << desc_.url
                        << " with a host resolution of " << host_ << ':'
                        << port_;
      resolver_.async_resolve(
          host_, port_,
          boost::beast::bind_front_handler(&HttpSession::on_resolve, me));
    } else {
      auto do_connect = [me]() {
        boost::beast::get_lowest_layer(me->stream_)
            .async_connect(me->resolution(), boost::beast::bind_front_handler(
                                                 &HttpSession::on_connect, me));
      };
      boost::asio::defer(strand_, do_connect);
    }
  }
  void on_resolve(boost::beast::error_code ec,
                  tcp::resolver::results_type results) {
    if (ec)
      return fail(ec, "resolve");
    resolution() = results;
    for (auto& ep : results) {
      GOOGLE_LOG(DEBUG) << desc_.url << " Resolved " << host_
                        << ", now connecting to " << req_[http::field::host]
                        << " aka " << ep.host_name() << ':' << ep.service_name()
                        << " for " << target_;
    }
    extend_time();
    boost::beast::get_lowest_layer(stream_).async_connect(
        results, boost::beast::bind_front_handler(&HttpSession::on_connect,
                                                  shared_from_this()));
  }

  void on_connect(boost::beast::error_code ec,
                  tcp::resolver::results_type::endpoint_type) {
    if (ec)
      return fail(ec, "connect");
    extend_time();
    GOOGLE_LOG(INFO) << desc_.url << " connected.";
    if (use_ssl()) {
      GOOGLE_LOG(DEBUG) << "Perform the SSL handshake because port=" << port_;
      stream_.async_handshake(
          boost::asio::ssl::stream_base::client,
          boost::beast::bind_front_handler(&HttpSession::on_handshake,
                                           shared_from_this()));
    } else {
      GOOGLE_LOG(DEBUG) << "Skipping the SSL handshake because port=" << port_;
      http::async_write(boost::beast::get_lowest_layer(stream_), req_,
                        boost::beast::bind_front_handler(&HttpSession::on_write,
                                                         shared_from_this()));
    }
  }
  bool use_ssl() const { return port_ == "443" || port_ == "https"; }
  void extend_time() {
    expiry_seconds_ += desc_.timeout_seconds + 1;
    GOOGLE_LOG(TRACE) << "expiry_seconds_ = " << expiry_seconds_ << '\n';
    boost::beast::get_lowest_layer(stream_).expires_after(
        std::chrono::seconds(expiry_seconds_));
  }
  void on_handshake(boost::beast::error_code ec) {
    if (ec)
      return fail(ec, "handshake");
    extend_time();
    http::async_write(stream_, req_,
                      boost::beast::bind_front_handler(&HttpSession::on_write,
                                                       shared_from_this()));
  }

  void on_write(boost::beast::error_code ec, std::size_t) {
    if (ec)
      return fail(ec, "write");
    GOOGLE_LOG(TRACE) << desc_.url << " request written.";
    extend_time();
    if (use_ssl()) {
      http::async_read(stream_, buffer_, response_parser_,
                       boost::beast::bind_front_handler(&HttpSession::on_read,
                                                        shared_from_this()));
    } else {
      http::async_read(boost::beast::get_lowest_layer(stream_), buffer_,
                       response_parser_,
                       boost::beast::bind_front_handler(&HttpSession::on_read,
                                                        shared_from_this()));
    }
  }

  void on_read(boost::beast::error_code ec, std::size_t bytes_transferred) {
    if (ec)
      return fail(ec, "read");
    res_ = response_parser_.release();
    ipfs::ContextApi::HeaderAccess get_hdr =
        [this](std::string_view k) -> std::string {
      std::string rv{(*res_)[k]};
      return rv;
    };
    GOOGLE_LOG(INFO) << "HTTP read (" << desc_.url << ";host=" << host_ << '('
                     << host_.size() << ");port=" << port_
                     << ";target=" << target_
                     << ": status=" << res_->result_int() << ", body is "
                     << bytes_transferred << "B, headers... ";
    if (res_->result_int() == 400) {
      GOOGLE_LOG(WARNING) << "Got that annoying 400 status: " << res_->body();
    }
    for (auto& h : *res_) {
      auto& n = h.name_string();
      if (n.substr(0, 6) != "Access") {
        GOOGLE_LOG(DEBUG) << "\t Header=" << h.name_string() << ": "
                          << h.value();
      }
    }
    if (res_->result_int() / 100 == 3) {
      auto loc = (*res_)[http::field::location];
      if (loc.empty()) {
        LOG(ERROR) << "No Location header given for a redirect response.";
      } else if (redirect_count(loc) >= 0xFF) {
        LOG(ERROR) << "Too many redirects!! Giving up on " << loc << '\n';
      } else {
        VLOG(1) << "Redirecting to " << loc << " aka " << desc_.url;
        auto desc = desc_;
        desc.url = loc;
        auto next = std::make_shared<HttpSession>(ioc_, ssl_ctx_, desc, cb_);
        next->prev = shared_from_this();
        next->run();
      }
      close();
      return;
    }
    auto content_type = (*res_)[http::field::content_type];
    auto me = shared_from_this();
    auto respond = [me, get_hdr]() {
      auto& r = *(me->res_);
      me->cb_(r.result_int(), r.body(), get_hdr);
    };
    if (content_type.empty() ||
        boost::algorithm::icontains(content_type, desc_.accept)) {
      LOG(TRACE) << "Got " << content_type;
    } else {
      LOG(INFO) << desc_.url
                << " response incorrect content type: " << content_type
                << " != " << desc_.accept;
      res_->result(501);
    }
    boost::asio::defer(strand_, respond);
    close();
  }
  int redirect_count(std::string_view comp) {
    if (comp == desc_.url) {
      LOG(ERROR) << "Redirect loop on " << comp;
      return 0xFF;
    } else if (!prev) {
      return 1;
    } else {
      return 1 + prev->redirect_count(comp);
    }
  }
  void close() {
    if (use_ssl()) {
      stream_.async_shutdown(boost::beast::bind_front_handler(
          &HttpSession::on_shutdown, shared_from_this()));
    } else {
      boost::beast::get_lowest_layer(stream_).close();
    }
  }
  void on_shutdown(boost::beast::error_code ec) {
    namespace E = boost::asio::error;
    switch (ec.value()) {
      case 0:
      case 1:
      case 2:
      case ENOTCONN:
        return;
      default:
        fail(ec, "shutdown");
    }
  }
};

std::map<std::string, boost::asio::ip::tcp::resolver::results_type>
    HttpSession::resolutions_;

void Self::SendHttpRequest(HttpRequestDescription desc,
                           HttpCompleteCallback cb) const {
  auto sess = std::make_shared<HttpSession>(io_, ssl_ctx_, desc, cb);
  sess->run();
}
std::optional<ipfs::GatewaySpec> Self::GetGateway(std::size_t index) const {
  if (index < gateways_.size()) {
    return gateways_.at(index);
  }
  return std::nullopt;
}
void Self::AddGateway(std::string_view prefix) {
  auto it = FindGateway(prefix);
  if (gateways_.end() != it && it->prefix == prefix) {
    it->rate++;
    LOG(INFO) << "Re-found existing gateway. Bumping it: " << prefix << '='
              << it->rate;
  } else {
    LOG(INFO) << "Adding new gateway:" << prefix;
    gateways_.insert(it, GatewaySpec{std::string{prefix}, 60U});
    DCHECK(
        std::is_sorted(gateways_.begin(), gateways_.end(),
                       [](auto& a, auto& b) { return a.prefix < b.prefix; }));
  }
}
void Self::SetGatewayRate(std::string_view prefix, unsigned int rate) {
  auto it = FindGateway(prefix);
  if (gateways_.end() != it && it->prefix == prefix) {
    LOG(INFO) << "Set gateway rate for " << prefix << " to " << rate;
    it->rate = rate;
  } else {
    LOG(INFO) << "Attempted to set the rate of an unknown gateway " << prefix
              << " to " << rate;
  }
}
unsigned Self::GetGatewayRate(std::string_view prefix) {
  auto i = FindGateway(prefix);
  return gateways_.end() == i ? 60U : i->rate;
}
auto Self::FindGateway(std::string_view prefix)
    -> std::vector<GatewaySpec>::iterator {
  auto cmp = [](auto& g, std::string_view p) { return g.prefix < p; };
  return std::lower_bound(gateways_.begin(), gateways_.end(), prefix, cmp);
}
#endif
