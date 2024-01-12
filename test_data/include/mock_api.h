#ifndef IPFS_MOCK_API_H_
#define IPFS_MOCK_API_H_

#include <gtest/gtest.h>

#include <ipfs_client/context_api.h>
#include <ipfs_client/json_cbor_adapter.h>

namespace ipfs::gw {
class GatewayRequest;
};

namespace i = ipfs;
namespace ig = i::gw;

using namespace std::literals;

namespace {
struct MockApi final : public i::ContextApi {
  std::vector<HttpRequestDescription> mutable http_requests_sent;
  std::vector<HttpCompleteCallback> mutable cbs;
  void SendHttpRequest(HttpRequestDescription d,
                       HttpCompleteCallback cb) const {
    http_requests_sent.push_back(d);
    cbs.push_back(cb);
  }
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
  std::string MimeType(std::string extension,
                       std::string_view content,
                       std::string const& url) const {
    return "";
  }
  std::string UnescapeUrlComponent(std::string_view url_comp) const {
    return "";
  }
  IpnsCborEntry deserialize_cbor(ByteView) const { return {}; }
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
  std::optional<i::GatewaySpec> GetGateway(std::size_t) const {
    return std::nullopt;
  }
  unsigned GetGatewayRate(std::string_view) { return 120U; }
  std::vector<std::string> gateways_added;
  void AddGateway(std::string_view g) { gateways_added.emplace_back(g); }
};
}  // namespace

#endif  // IPFS_MOCK_API_H_
