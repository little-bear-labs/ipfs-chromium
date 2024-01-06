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
#include <boost/beast/ssl.hpp>

namespace google::protobuf {
constexpr LogLevel LOGLEVEL_DEBUG = static_cast<LogLevel>(-1);
constexpr LogLevel LOGLEVEL_TRACE = static_cast<LogLevel>(-2);
}  // namespace google::protobuf

class HttpSession;

namespace ipfs {

// LCOV_EXCL_START

class TestContext final : public ContextApi {
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
  std::unique_ptr<DagJsonValue> ParseJson(
      std::string_view j_str) const override {
    auto data = nlohmann::json::parse(j_str);
    std::ostringstream oss;
    oss << std::setw(2) << data;
    GOOGLE_LOG(DEBUG) << "Parsed " << j_str.size()
                      << " bytes of JSON string and got " << oss.str();
    return std::make_unique<ipfs::JsonCborAdapter>(data);
  }
  bool VerifyKeySignature(SigningKeyType,
                          ByteView,
                          ByteView,
                          ByteView) const override {
    GOOGLE_LOG(ERROR) << "TODO\n";
    return true;
  }

  std::vector<GatewaySpec> gateways_;
  boost::asio::io_context& io_;
  boost::asio::ssl::context mutable ssl_ctx_ =
      boost::asio::ssl::context{boost::asio::ssl::context::tls_client};
  //      boost::asio::ssl::context{boost::asio::ssl::context::tlsv13_client};
  ares_channel_t* ares_channel_ = nullptr;
  void CAresProcess();

 public:
  TestContext(boost::asio::io_context& io);
  ~TestContext() noexcept override;
  void DnsResults(std::string&, ares_txt_reply&);
  std::optional<ipfs::GatewaySpec> GetGateway(std::size_t) const;
  void AddGateway(std::string_view) override;
  void SetGatewayRate(std::string_view, unsigned);
  unsigned int GetGatewayRate(std::string_view) override;
  std::vector<GatewaySpec>::iterator FindGateway(std::string_view);
};
inline std::pair<std::shared_ptr<TestContext>, std::shared_ptr<Orchestrator>>
start_default(boost::asio::io_context& io) {
  auto api = std::make_shared<TestContext>(io);
  auto gl = Gateways::DefaultGateways();
  auto rtor = gw::default_requestor(gl, {}, api);
  auto orc = std::make_shared<Orchestrator>(rtor, api);
  return {api, orc};
}

}  // namespace ipfs

#endif  // boost

#endif  //_MSC_VER
#endif  // IPFS_ALL_INCLUSIVE_CONTEXT_H_
