#ifndef IPFS_MOCK_API_H_
#define IPFS_MOCK_API_H_

#include <gtest/gtest.h>

#include <ipfs_client/context_api.h>
#include <ipfs_client/json_cbor_adapter.h>

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
#if HAS_JSON_CBOR_ADAPTER
  std::unique_ptr<ipfs::DagCborValue> ParseCbor(ByteView bv) const {
    return std::make_unique<ipfs::JsonCborAdapter>(nlohmann::json::from_cbor(bv));
  }
#else
  std::unique_ptr<ipfs::DagCborValue> ParseCbor(ByteView bv) const { return {}; }
#endif
};
}  // namespace

#endif  // IPFS_MOCK_API_H_
