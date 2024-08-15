#include <ipfs_client/ctx/boost_beast_http.h>

#include <vocab/slash_delimited.h>

#include "log_macros.h"

#if HAS_BOOST_BEAST

#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>

#include <map>

using Self = ipfs::ctx::BoostBeastHttp;

Self::BoostBeastHttp(boost::asio::io_context& c) : io_{c} {}

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
  using api = ipfs::ctx::HttpApi;
  api::OnComplete cb_;
  int expiry_seconds_ = 91;
  std::string host_, port_, target_;
  ipfs::HttpRequestDescription desc_;
  static std::map<std::string, tcp::resolver::results_type> resolutions_;
  std::string parsed_host_;
  http::response_parser<http::string_body> response_parser_;
  std::optional<http::response<http::string_body>> res_;
  std::shared_ptr<HttpSession> prev;

  void fail(boost::beast::error_code ec, char const* what) {
    LOG(INFO) << what << ": " << ec.value() << ' ' << ec.message()
              << " URL:" << desc_.url << " HOST:" << host_ << " PORT:" << port_
              << " TARGET:" << target_;
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
                       api::OnComplete cb)
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
      LOG(ERROR) << "SSL early fail: " << ec.message();
      return;
    }

    req_.version(11);
    req_.method(http::verb::get);
    req_.target(target_);
    req_.set(http::field::host, parsed_host_);
    if (desc_.accept.size()) {
      req_.set("Accept", desc_.accept);
    }
    extend_time();
    auto me = shared_from_this();
    if (resolution().empty()) {
      VLOG(1) << "Starting " << desc_.url << " with a host resolution of "
              << host_ << ':' << port_;
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
      VLOG(1) << desc_.url << " Resolved " << host_ << ", now connecting to "
              << req_[http::field::host] << " aka " << ep.host_name() << ':'
              << ep.service_name() << " for " << target_;
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
    LOG(INFO) << desc_.url << " connected.";
    if (use_ssl()) {
      VLOG(1) << "Perform the SSL handshake because port=" << port_;
      stream_.async_handshake(
          boost::asio::ssl::stream_base::client,
          boost::beast::bind_front_handler(&HttpSession::on_handshake,
                                           shared_from_this()));
    } else {
      VLOG(1) << "Skipping the SSL handshake because port=" << port_;
      http::async_write(boost::beast::get_lowest_layer(stream_), req_,
                        boost::beast::bind_front_handler(&HttpSession::on_write,
                                                         shared_from_this()));
    }
  }
  bool use_ssl() const { return port_ == "443" || port_ == "https"; }
  void extend_time() {
    expiry_seconds_ += desc_.timeout_seconds + 1;
    VLOG(2) << "expiry_seconds_ = " << expiry_seconds_ << '\n';
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
    VLOG(2) << desc_.url << " request written.";
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
    api::Hdrs get_hdr = [this](std::string_view k) -> std::string {
      std::string rv{(*res_)[k]};
      return rv;
    };
    LOG(INFO) << "HTTP read (" << desc_.url << ";host=" << host_ << '('
              << host_.size() << ");port=" << port_ << ";target=" << target_
              << ": status=" << res_->result_int() << ", body is "
              << bytes_transferred << "B, headers... ";
    if (res_->result_int() == 400) {
      LOG(WARNING) << "Got that annoying 400 status: " << res_->body();
    }
    for (auto& h : *res_) {
      auto& n = h.name_string();
      if (n.substr(0, 6) != "Access") {
        VLOG(1) << "\t Header=" << h.name_string() << ": " << h.value();
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
      VLOG(2) << "Got " << content_type;
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
  bool closed_ = false;
  void close() {
    if (closed_) {
      return;
    }
    closed_ = true;
    boost::system::error_code ec;
    stream_.shutdown(ec);
    auto& ll = boost::beast::get_lowest_layer(stream_);
    ll.cancel();
    ll.close();
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

auto Self::SendHttpRequest(ReqDesc desc, OnComplete cb) const -> Canceller {
  auto sess = std::make_shared<HttpSession>(io_, ssl_ctx_, desc, cb);
  sess->run();
  std::weak_ptr<HttpSession> w = sess;
  return [w](){
    if (auto p = w.lock()) {
      p->close();
    }
  };
}

#endif
