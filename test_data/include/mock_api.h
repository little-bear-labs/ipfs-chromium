#ifndef IPFS_MOCK_API_H_
#define IPFS_MOCK_API_H_

#include <gtest/gtest.h>

#include <ipfs_client/client.h>
#include <ipfs_client/json_cbor_adapter.h>

#include "mock_gw_cfg.h"
#include "mock_sig_vtor.h"

namespace ipfs::gw {
class GatewayRequest;
};

namespace i = ipfs;
namespace ig = i::gw;
namespace ic = i::ctx;
namespace ii = i::ipld;
using RT = ig::GatewayRequestType;

using namespace std::literals;

namespace {
struct MockDnsTxt : public i::ctx::DnsTxtLookup {
  struct DnsInvocation {
    std::string host;
    std::vector<std::string> txt_records;
  };

  std::vector<DnsInvocation> mutable expected_dns;
  void SendDnsTextRequest(std::string host,
                          DnsTextResultsCallback rcb,
                          DnsTextCompleteCallback ccb) {
    EXPECT_GE(expected_dns.size(), 1U);
    auto& e = expected_dns.at(0);
    EXPECT_EQ(e.host, host);
    rcb(e.txt_records);
    expected_dns.erase(expected_dns.begin());
    ccb();
  }
};
struct MockApi final : public i::Client {
  MockDnsTxt* dns_;
  MockGwCfg* gw_;
  MockApi() {
    auto dns = std::make_unique<MockDnsTxt>();
    dns_ = dns.get();
    with(std::move(dns));
    auto g = std::make_unique<MockGwCfg>();
    gw_ = g.get();
    with(std::move(g));
    with(i::crypto::SigningKeyType::RSA, std::make_unique<MockSigVtor>());
    with(i::crypto::SigningKeyType::Ed25519, std::make_unique<MockSigVtor>());
  }
  ~MockApi() noexcept override {}
  std::vector<i::HttpRequestDescription> mutable http_requests_sent;
  std::vector<ic::HttpApi::HttpCompleteCallback> mutable cbs;
  std::string MimeType(std::string extension,
                       std::string_view content,
                       std::string const& url) const {
    return "";
  }
  std::string UnescapeUrlComponent(std::string_view url_comp) const {
    return "";
  }
  bool VerifyKeySignature(SigningKeyType,
                          ByteView signature,
                            ByteView data,
                            ByteView key_bytes) const {
    return true;
  }

  void Discover(std::function<void(std::vector<std::string>)> cb) {}
#if HAS_JSON_CBOR_ADAPTER
  std::unique_ptr<ipfs::DagCborValue> ParseCbor(ByteView bv) const {
    return std::make_unique<ipfs::JsonCborAdapter>(nlohmann::json::from_cbor(
        bv, false, true, nlohmann::detail::cbor_tag_handler_t::store));
  }
  std::unique_ptr<ipfs::DagJsonValue> ParseJson(std::string_view jstr) const {
    return std::make_unique<ipfs::JsonCborAdapter>(nlohmann::json::parse(jstr));
  }
#else
  std::unique_ptr<ipfs::DagCborValue> ParseCbor(ByteView bv) const {
    return {};
  }
  std::unique_ptr<ipfs::DagJsonValue> ParseJson(std::string_view) const {
    return {};
  }
#endif
};
}  // namespace

#endif  // IPFS_MOCK_API_H_
