#include <ipfs_client/gw/gateway_http_requestor.h>

#include <gtest/gtest.h>

#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/orchestrator.h>

using T = ipfs::gw::GatewayHttpRequestor;
using R = ipfs::gw::GatewayRequest;
using P = ipfs::gw::RequestPtr;

using namespace std::literals;

namespace {
struct FakeApi final : public ipfs::ContextApi {
  std::vector<HttpRequestDescription> mutable http_requests_sent;
  std::vector<HttpCompleteCallback> mutable cbs;
  void SendHttpRequest(HttpRequestDescription d,
                       HttpCompleteCallback cb) const {
    http_requests_sent.push_back(d);
    cbs.push_back(cb);
  }

  void SendDnsTextRequest(std::string hostname,
                          DnsTextResultsCallback,
                          DnsTextCompleteCallback) {}
  std::string MimeType(std::string extension,
                       std::string_view content,
                       std::string const& url) const {
    return "";
  }
  std::string UnescapeUrlComponent(std::string_view url_comp) const {
    return "";
  }
  IpnsCborEntry deserialize_cbor(ByteView) const { return {}; }
  bool verify_key_signature(SigningKeyType,
                            ByteView signature,
                            ByteView data,
                            ByteView key_bytes) const {
    return true;
  }

  void Discover(std::function<void(std::vector<std::string>)> cb) {}
  std::shared_ptr<ipfs::GatewayRequest> InitiateGatewayRequest(
      ipfs::BusyGateway) {
    return {};
  }
};
struct FakeRtor : public ipfs::gw::Requestor {
  std::string_view name() const { return "fake requestor"; }
  std::vector<P> requests_forwarded;
  HandleOutcome handle(P p) {
    requests_forwarded.push_back(p);
    return HandleOutcome::DONE;
  }
};
struct GatewayHttpRequestorTest : public ::testing::Test {
  std::shared_ptr<FakeApi> api = std::make_shared<FakeApi>();
  std::shared_ptr<T> t = std::make_shared<T>("scheme://host", api);
  std::shared_ptr<FakeRtor> chain = std::make_shared<FakeRtor>();
  std::shared_ptr<ipfs::Orchestrator> orc;
  P req() {
    t->or_else(chain);
    auto noop = [](auto) {};
    auto noop2 = [](auto, auto) {};
    auto req = R::fromIpfsPath(
        "/ipfs/bafkreihdwdcefgh4dqkjv67uzcmw7ojee6xedzdetojuzjevtenxquvyku"sv);
    req->orchestrator = orc =
        std::make_shared<ipfs::Orchestrator>(noop, t, api);
    req->dependent = std::make_shared<ipfs::IpfsRequest>(
        "/ipfs/bafkreihdwdcefgh4dqkjv67uzcmw7ojee6xedzdetojuzjevtenxquvyku",
        noop2);
    return req;
  }
  ipfs::gw::Requestor& sup() { return *t; }
};
}  // namespace
TEST_F(GatewayHttpRequestorTest, name) {
  EXPECT_EQ(sup().name(), "simplistic HTTP requestor");
}
TEST_F(GatewayHttpRequestorTest, without_slash) {
  t->request(req());
  EXPECT_EQ(api->http_requests_sent.size(), 1);
  EXPECT_EQ(api->http_requests_sent.at(0).url,
            "scheme://host/ipfs/"
            "bafkreihdwdcefgh4dqkjv67uzcmw7ojee6xedzdetojuzjevtenxquvyku");
}
TEST_F(GatewayHttpRequestorTest, with_slash) {
  t = std::make_shared<T>("scheme://host/", api);
  t->request(req());
  EXPECT_EQ(api->http_requests_sent.size(), 1);
  EXPECT_EQ(api->http_requests_sent.at(0).url,
            "scheme://host/ipfs/"
            "bafkreihdwdcefgh4dqkjv67uzcmw7ojee6xedzdetojuzjevtenxquvyku");
}
TEST_F(GatewayHttpRequestorTest, block_has_accept) {
  t->request(req());
  EXPECT_EQ(api->http_requests_sent.size(), 1);
  EXPECT_EQ(api->http_requests_sent.at(0).accept, "application/vnd.ipld.raw");
}
TEST_F(GatewayHttpRequestorTest, callback_failure_forwards) {
  t->request(req());
  EXPECT_EQ(api->http_requests_sent.size(), 1);
  EXPECT_EQ(api->http_requests_sent.at(0).accept, "application/vnd.ipld.raw");
  EXPECT_EQ(api->cbs.size(), 1);
  auto noop = [](auto) { return std::string{}; };
  api->cbs.at(0)(404, "", noop);
  EXPECT_EQ(chain->requests_forwarded.size(), 1);
}
TEST_F(GatewayHttpRequestorTest, callback_success_adds_node) {
  t->request(req());
  EXPECT_EQ(api->http_requests_sent.size(), 1);
  EXPECT_EQ(api->http_requests_sent.at(0).accept, "application/vnd.ipld.raw");
  EXPECT_EQ(api->cbs.size(), 1);
  auto noop = [](auto) { return std::string{}; };
  api->cbs.at(0)(200, "", noop);
  EXPECT_TRUE(orc->has_key(
      "bafkreihdwdcefgh4dqkjv67uzcmw7ojee6xedzdetojuzjevtenxquvyku"));
  EXPECT_EQ(chain->requests_forwarded.size(), 0);
}
TEST_F(GatewayHttpRequestorTest, high_parallel_means_maybe_later) {
  auto r = req();
  r->parallel = 8;
  t->request(r);
  EXPECT_EQ(api->http_requests_sent.size(), 0);
  EXPECT_EQ(api->cbs.size(), 0);
  EXPECT_EQ(chain->requests_forwarded.size(), 1);
}
TEST_F(GatewayHttpRequestorTest, high_parallel_passeswithaffinity) {
  auto r = req();
  EXPECT_TRUE(orc);
  t->request(r);
  EXPECT_EQ(api->http_requests_sent.size(), 1);
  EXPECT_EQ(api->cbs.size(), 1);
  EXPECT_EQ(chain->requests_forwarded.size(), 0);
  EXPECT_EQ(api->http_requests_sent.at(0).url,
            "scheme://host/ipfs/"
            "bafkreihdwdcefgh4dqkjv67uzcmw7ojee6xedzdetojuzjevtenxquvyku");
  auto noop = [](auto) { return std::string{}; };
  api->cbs.at(0)(200, "", noop);
  r->parallel = 8;
  r->type = ipfs::gw::Type::Car;
  r->path = "index.html";
  t->request(r);
  EXPECT_EQ(api->http_requests_sent.size(), 2);
  EXPECT_EQ(api->cbs.size(), 2);
  EXPECT_EQ(chain->requests_forwarded.size(), 0);
  EXPECT_EQ(api->http_requests_sent.at(1).url,
            "scheme://host/ipfs/"
            "bafkreihdwdcefgh4dqkjv67uzcmw7ojee6xedzdetojuzjevtenxquvyku/"
            "index.html?dag-scope=entity");
}
