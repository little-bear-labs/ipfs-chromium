#ifndef IPFS_ALL_INCLUSIVE_CONTEXT_H_
#define IPFS_ALL_INCLUSIVE_CONTEXT_H_

#ifdef _MSC_VER
// #warning "AllInclusiveContext has not been ported to Windows."
#else

#include "gw/default_requestor.h"

#include "context_api.h"
#include "dag_cbor_value.h"
#include "gateways.h"
#include "json_cbor_adapter.h"
#include "orchestrator.h"

#include <vocab/slash_delimited.h>

#include <google/protobuf/stubs/logging.h>

#include <cassert>

#if ! __has_include(<boost/asio/strand.hpp>)
#warning "One needs access to Boost to use this header"
#elif !__has_include(<ares.h>)
#warning "One needs c-ares available to use this header."
#elif !HAS_JSON_CBOR_ADAPTER
#warning "One needs access to nlohmann/json to use this header"
#else

#define HAS_ALL_INCLUSIVE 1

#include <ares.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http.hpp>

namespace google::protobuf {
constexpr LogLevel LOGLEVEL_DEBUG = static_cast<LogLevel>(-1);
constexpr LogLevel LOGLEVEL_TRACE = static_cast<LogLevel>(-2);
}  // namespace google::protobuf

namespace ipfs {

class HttpSession;

class AllInclusiveContext final : public ContextApi {
  void SendHttpRequest(HttpRequestDescription,
                       HttpCompleteCallback) const override;
  struct DnsCbs {
    DnsTextResultsCallback r;
    DnsTextCompleteCallback c;
  };
  std::map<std::string, std::vector<DnsCbs>> pending_dns_;
  void SendDnsTextRequest(std::string,
                          DnsTextResultsCallback,
                          DnsTextCompleteCallback) override;
  std::string MimeType(std::string extension,
                       std::string_view,
                       std::string const&) const override {
    // TODO implement real mime type detection
    return "text/" + extension;
  }
  std::string UnescapeUrlComponent(std::string_view url_comp) const override {
    std::string rv{url_comp};
    auto xval = [](char c) {
      if (c <= '9') {
        return c - '0';
      }
      if (c <= 'Z') {
        return c - 'A';
      }
      return c - 'a';
    };
    for (auto i = 0UL; i + 1UL < rv.size(); ++i) {
      if (rv[i] != '%') {
        continue;
      }
      auto a = rv[i + 1UL];
      if (rv[i + 1UL] == '%') {
        rv.erase(i, 1UL);
        continue;
      }
      if (i + 2UL >= rv.size()) {
        break;
      }
      if (!std::isxdigit(a)) {
        continue;
      }
      auto b = rv[i + 2UL];
      if (std::isxdigit(b)) {
        rv[i] = xval(a) * 16 + xval(b);
        rv.erase(i + 1UL, 2);
      }
    }
    return rv;
  }
  std::unique_ptr<DagCborValue> ParseCbor(ByteView bytes) const override {
    auto data = nlohmann::json::from_cbor(
        bytes, false, true, nlohmann::detail::cbor_tag_handler_t::store);
    return std::make_unique<ipfs::JsonCborAdapter>(data);
  }
  bool verify_key_signature(SigningKeyType,
                            ByteView,
                            ByteView,
                            ByteView) const override {
    GOOGLE_LOG(ERROR) << "TODO\n";
    return true;
  }
  boost::asio::io_context& io_;
  ares_channel_t* ares_channel_ = nullptr;
  void CAresProcess();

 public:
  AllInclusiveContext(boost::asio::io_context& io);
  ~AllInclusiveContext() noexcept override;
  void DnsResults(std::string&, ares_txt_reply&);
};
class HttpSession : public std::enable_shared_from_this<HttpSession> {
  using tcp = boost::asio::ip::tcp;
  tcp::resolver resolver_;
  boost::beast::tcp_stream stream_;
  boost::beast::flat_buffer buffer_;  // (Must persist between reads)
  boost::beast::http::request<boost::beast::http::empty_body> req_;
  boost::beast::http::response<boost::beast::http::string_body> res_;
  ipfs::ContextApi::HttpCompleteCallback cb_;
  int expiry_seconds_ = 91;
  std::string host_, port_, target_;
  ipfs::HttpRequestDescription desc_;
  tcp::resolver::results_type resolution_;
  std::string parsed_host_;

  void fail(boost::beast::error_code ec, char const* what) {
    std::cout << "\n\tERROR:\t" << what << ": " << ec.message() << ' '
              << desc_.url << ' ' << host_ << ' ' << port_ << ' ' << target_
              << std::endl;
    cb_(500, "", [](auto) { return std::string{}; });
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
                       ipfs::HttpRequestDescription& desc,
                       ipfs::ContextApi::HttpCompleteCallback cb)
      : resolver_(boost::asio::make_strand(ioc)),
        stream_(boost::asio::make_strand(ioc)),
        cb_{cb},
        desc_{desc} {}

  // Start the asynchronous operation
  void run() {
    auto parsed_host_ = parse_url();
    if (host_.empty()) {
      host_ = parsed_host_;
    }
    expiry_seconds_ += desc_.timeout_seconds;
    GOOGLE_LOG(DEBUG) << "expiry_seconds_ = " << expiry_seconds_ << '\n';
    // Set up an HTTP GET request message
    req_.version(11);
    req_.method(boost::beast::http::verb::get);
    req_.target(target_);
    req_.set(boost::beast::http::field::host, parsed_host_);
    if (desc_.accept.size()) {
      //      std::clog << "Setting Accept: " << desc_.accept << '\n';
      req_.set("Accept", desc_.accept);
    }
    GOOGLE_LOG(DEBUG) << "Starting " << desc_.url
                      << " with a host resolution of " << host_ << ':' << port_;
    // Look up the domain name
    resolver_.async_resolve(host_, port_,
                            boost::beast::bind_front_handler(
                                &HttpSession::on_resolve, shared_from_this()));
  }

  void on_resolve(boost::beast::error_code ec,
                  tcp::resolver::results_type results) {
    if (ec)
      return fail(ec, "resolve");
    resolution_ = results;
    // Set a timeout on the operation
    stream_.expires_after(std::chrono::seconds(expiry_seconds_));
    GOOGLE_LOG(DEBUG) << desc_.url << " Resolved " << host_
                      << ", now connecting to "
                      << req_[boost::beast::http::field::host] << " for "
                      << target_;
    // Make the connection on the IP address we get from a lookup
    stream_.async_connect(
        results, boost::beast::bind_front_handler(&HttpSession::on_connect,
                                                  shared_from_this()));
  }

  void on_connect(boost::beast::error_code ec,
                  tcp::resolver::results_type::endpoint_type) {
    if (ec)
      return fail(ec, "connect");

    // Set a timeout on the operation
    stream_.expires_after(std::chrono::seconds(expiry_seconds_));
    GOOGLE_LOG(TRACE) << desc_.url << " connected.";
    // Send the HTTP request to the remote host
    boost::beast::http::async_write(
        stream_, req_,
        boost::beast::bind_front_handler(&HttpSession::on_write,
                                         shared_from_this()));
  }

  void on_write(boost::beast::error_code ec, std::size_t) {
    if (ec)
      return fail(ec, "write");
    GOOGLE_LOG(TRACE) << desc_.url << " request written.";
    // Receive the HTTP response
    boost::beast::http::async_read(
        stream_, buffer_, res_,
        boost::beast::bind_front_handler(&HttpSession::on_read,
                                         shared_from_this()));
  }

  void on_read(boost::beast::error_code ec, std::size_t bytes_transferred) {
    if (ec)
      return fail(ec, "read");
    ipfs::ContextApi::HeaderAccess get_hdr =
        [this](std::string_view k) -> std::string {
      std::string rv{res_[k]};
      return rv;
    };
    GOOGLE_LOG(INFO) << "HTTP read successful(" << desc_.url
                     << ";host=" << host_ << '(' << host_.size()
                     << ");port=" << port_ << ";target=" << target_
                     << ": status=" << res_.result_int() << ", body is "
                     << bytes_transferred << "B, headers... ";
    for (auto& h : res_) {
      auto& n = h.name_string();
      if (n.substr(0, 6) != "Access") {
        GOOGLE_LOG(INFO) << "\t Header=" << h.name_string() << ": "
                         << h.value();
      }
    }
    if (res_.result_int() / 100 == 3) {
      auto loc = res_[boost::beast::http::field::location];
      if (loc.size()) {
        GOOGLE_LOG(WARNING) << "Redirecting to " << loc;
        res_ = boost::beast::http::response<boost::beast::http::string_body>{};
        desc_.url = loc;
        req_.set(boost::beast::http::field::host, parse_url());
        req_.target(target_);
        on_resolve({}, resolution_);
        return;
      }
    }
    auto content_type = res_[boost::beast::http::field::content_type];
    if (content_type.empty() ||
        boost::algorithm::icontains(content_type, desc_.accept)) {
      cb_(res_.result_int(), res_.body(), get_hdr);
    } else {
      GOOGLE_LOG(INFO) << desc_.url
                       << " response incorrect content type: " << content_type
                       << " != " << desc_.accept;
    }

    // Gracefully close the socket
    stream_.socket().shutdown(tcp::socket::shutdown_both, ec);

    // not_connected happens sometimes so don't bother reporting it.
    if (ec && ec != boost::beast::errc::not_connected)
      return fail(ec, "shutdown");

    // If we get here then the connection is closed gracefully
  }
};

inline std::shared_ptr<Orchestrator> start_default(
    boost::asio::io_context& io) {
  auto api = std::make_shared<AllInclusiveContext>(io);
  auto gl = Gateways::DefaultGateways();
  auto rtor = gw::default_requestor(gl, {}, api);
  auto orc = std::make_shared<Orchestrator>(rtor, api);
  return orc;
}

}  // namespace ipfs

#endif  // boost

#endif  //_MSC_VER
#endif  // IPFS_ALL_INCLUSIVE_CONTEXT_H_
