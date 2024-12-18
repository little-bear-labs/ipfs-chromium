#include <ipfs_client/ipns_record.h>

#include <mock_api.h>
#include <mock_cbor.h>
#include <log_recorder.h>

#include <ipfs_client/cid.h>
#include <ipfs_client/dag_json_value.h>
#include <ipfs_client/logger.h>

#include <list>

namespace i = ipfs;
namespace il= i::log;

namespace {
struct Api final : public i::Client {
  std::string MimeType(std::string extension,
                       std::string_view content,
                       std::string const& url) const {
    throw 1;
  }
  bool VerifyKeySignature(SigningKeyType,
                          ByteView signature,
                          ByteView data,
                          ByteView key_bytes) const {
    return true;
  }
  std::string UnescapeUrlComponent(std::string_view url_comp) const { throw 1; }
  ipfs::IpnsCborEntry deserialize_cbor(ipfs::ByteView b) const {
    return dser_(b);
  }
  std::list<std::unique_ptr<MockCbor>> mutable cbors;
  std::unique_ptr<ipfs::DagCborValue> ParseCbor(ByteView) const {
    if (cbors.empty()) {
      return {};
    }
    auto r = std::move(cbors.front());
    cbors.pop_front();
    return std::move(r);
  }
  bool verify_key_signature(ipfs::crypto::SigningKeyType,
                            ipfs::ByteView signature,
                            ipfs::ByteView data,
                            ipfs::ByteView key_bytes) {
    return true;
  }
  void Discover(std::function<void(std::vector<std::string>)> cb) { throw 1; }
  std::unique_ptr<ipfs::DagJsonValue> ParseJson(std::string_view) const {
    return {};
  }
  std::function<ipfs::IpnsCborEntry(ipfs::ByteView)> dser_;
  Api(std::function<ipfs::IpnsCborEntry(ipfs::ByteView)> f = {}) {
    with(i::crypto::SigningKeyType::RSA, std::make_unique<MockSigVtor>());
    if (f) {
      dser_ = f;
    } else {
      dser_ = [](auto) {
        ipfs::IpnsCborEntry e;
        e.value =
            "/ipfs/bafybeig57t2dp435aupttilimd6767kppfebaa3gnunmqden66dgkhugwi";
        e.validity = "2023-03-24T05:10:02.161162321Z";
        e.validityType = 0;
        e.sequence = 384;
        e.ttl = 60000000000;
        return e;
      };
    }
  }
  std::optional<i::GatewaySpec> GetGateway(std::size_t) const {
    return std::nullopt;
  }
  unsigned GetGatewayRate(std::string_view) { return 120U; }
  void AddGateway(std::string_view) {}
  virtual ~Api() noexcept {}
};
  std::vector<std::string> log_lines;
  std::string all_log;
  void save_log(std::string const& msg, char const*, int, il::Level) {
    log_lines.push_back(msg);
    all_log.append(msg).push_back(';');
  }
}  // namespace

TEST(IpnsRecordTest, AKnownKuboRecord) {
  Api api;
  std::array<std::uint8_t, 400> xxd{
      0x0a, 0x41, 0x2f, 0x69, 0x70, 0x66, 0x73, 0x2f, 0x62, 0x61, 0x66, 0x79,
      0x62, 0x65, 0x69, 0x67, 0x35, 0x37, 0x74, 0x32, 0x64, 0x70, 0x34, 0x33,
      0x35, 0x61, 0x75, 0x70, 0x74, 0x74, 0x69, 0x6c, 0x69, 0x6d, 0x64, 0x36,
      0x37, 0x36, 0x37, 0x6b, 0x70, 0x70, 0x66, 0x65, 0x62, 0x61, 0x61, 0x33,
      0x67, 0x6e, 0x75, 0x6e, 0x6d, 0x71, 0x64, 0x65, 0x6e, 0x36, 0x36, 0x64,
      0x67, 0x6b, 0x68, 0x75, 0x67, 0x77, 0x69, 0x12, 0x40, 0xba, 0xde, 0xd1,
      0xdb, 0x61, 0xc6, 0x41, 0x84, 0xfb, 0x62, 0x8b, 0xff, 0x2d, 0x1d, 0x24,
      0x33, 0xb9, 0x25, 0x95, 0xe0, 0x27, 0x27, 0x59, 0x8b, 0x32, 0xae, 0x61,
      0x70, 0x79, 0x94, 0xd2, 0x79, 0xb7, 0x58, 0x46, 0x1e, 0x18, 0x12, 0xb5,
      0xaa, 0xcf, 0x6c, 0x09, 0x7b, 0xf7, 0xad, 0xd0, 0xbe, 0xa7, 0x76, 0x49,
      0x02, 0x6a, 0xd3, 0x82, 0x3b, 0xc8, 0x18, 0x45, 0xc6, 0x35, 0x22, 0xd3,
      0x0d, 0x18, 0x00, 0x22, 0x1e, 0x32, 0x30, 0x32, 0x33, 0x2d, 0x30, 0x33,
      0x2d, 0x32, 0x34, 0x54, 0x30, 0x35, 0x3a, 0x31, 0x30, 0x3a, 0x30, 0x32,
      0x2e, 0x31, 0x36, 0x31, 0x31, 0x36, 0x32, 0x33, 0x32, 0x31, 0x5a, 0x28,
      0x80, 0x03, 0x30, 0x80, 0xb0, 0x9d, 0xc2, 0xdf, 0x01, 0x42, 0x40, 0xfc,
      0x7f, 0xa0, 0x06, 0xe8, 0x36, 0x03, 0xef, 0x1b, 0x5d, 0x3d, 0x9a, 0x57,
      0x74, 0xcf, 0xce, 0x92, 0x83, 0x3d, 0xc6, 0x2b, 0x93, 0x54, 0x11, 0x6e,
      0x7e, 0xcb, 0x1e, 0x2c, 0x8b, 0xc6, 0x09, 0x8e, 0xe3, 0xb6, 0xb0, 0x0e,
      0xaa, 0x2a, 0x3f, 0xee, 0xe1, 0x29, 0xe4, 0x6d, 0x4f, 0xf7, 0x40, 0x8f,
      0x1c, 0x89, 0x05, 0x01, 0x9a, 0x67, 0xf0, 0xa5, 0x7c, 0x34, 0xf0, 0xe3,
      0xb8, 0xb0, 0x0c, 0x4a, 0x9a, 0x01, 0xa5, 0x63, 0x54, 0x54, 0x4c, 0x1b,
      0x00, 0x00, 0x00, 0x0d, 0xf8, 0x47, 0x58, 0x00, 0x65, 0x56, 0x61, 0x6c,
      0x75, 0x65, 0x58, 0x41, 0x2f, 0x69, 0x70, 0x66, 0x73, 0x2f, 0x62, 0x61,
      0x66, 0x79, 0x62, 0x65, 0x69, 0x67, 0x35, 0x37, 0x74, 0x32, 0x64, 0x70,
      0x34, 0x33, 0x35, 0x61, 0x75, 0x70, 0x74, 0x74, 0x69, 0x6c, 0x69, 0x6d,
      0x64, 0x36, 0x37, 0x36, 0x37, 0x6b, 0x70, 0x70, 0x66, 0x65, 0x62, 0x61,
      0x61, 0x33, 0x67, 0x6e, 0x75, 0x6e, 0x6d, 0x71, 0x64, 0x65, 0x6e, 0x36,
      0x36, 0x64, 0x67, 0x6b, 0x68, 0x75, 0x67, 0x77, 0x69, 0x68, 0x53, 0x65,
      0x71, 0x75, 0x65, 0x6e, 0x63, 0x65, 0x19, 0x01, 0x80, 0x68, 0x56, 0x61,
      0x6c, 0x69, 0x64, 0x69, 0x74, 0x79, 0x58, 0x1e, 0x32, 0x30, 0x32, 0x33,
      0x2d, 0x30, 0x33, 0x2d, 0x32, 0x34, 0x54, 0x30, 0x35, 0x3a, 0x31, 0x30,
      0x3a, 0x30, 0x32, 0x2e, 0x31, 0x36, 0x31, 0x31, 0x36, 0x32, 0x33, 0x32,
      0x31, 0x5a, 0x6c, 0x56, 0x61, 0x6c, 0x69, 0x64, 0x69, 0x74, 0x79, 0x54,
      0x79, 0x70, 0x65, 0x00};
  ipfs::ByteView known_record{reinterpret_cast<ipfs::Byte const*>(xxd.data()),
                              xxd.size()};
  ipfs::Cid ci{
      "bafzaajaiaejcaxykhmgsz2mhscluhm6bkliibattya2l2lld7scqr64c4ine2u7c"};
  auto cbor = std::make_unique<MockCbor>();
  cbor->set(
      "Value",
      "/ipfs/bafybeig57t2dp435aupttilimd6767kppfebaa3gnunmqden66dgkhugwi"s);
  cbor->set("Validity", "2023-03-24T05:10:02.161162321Z"s);
  cbor->set("ValidityType", 0UL);
  cbor->set("Sequence", 384UL);
  cbor->set("TTL", 60000000000UL);
  api.cbors.emplace_back(std::move(cbor));
  auto result = ipfs::ValidateIpnsRecord(known_record, ci, api);
  std::string_view expected{
      "/ipfs/bafybeig57t2dp435aupttilimd6767kppfebaa3gnunmqden66dgkhugwi"};
  EXPECT_EQ(result.has_value(), true);
  EXPECT_EQ(result.value().value, expected);
}
TEST(IpnsRecordTest, SerializeValidatedIpns) {
  ipfs::ValidatedIpns v;
  v.sequence = 1;
  v.use_until = 2;
  v.cache_until = 3;
  v.fetch_time = 4;
  v.resolution_ms = 5;
  v.gateway_source = "gateway_source";
  v.value = "value";
  auto actual = v.Serialize();
  auto expected = "1 2 3 4 5 value gateway_source";
  EXPECT_EQ(actual, expected);
  auto w = ipfs::ValidatedIpns::Deserialize(actual);
  EXPECT_EQ(w.sequence, v.sequence);
  EXPECT_EQ(w.use_until, v.use_until);
  EXPECT_EQ(w.cache_until, v.cache_until);
  EXPECT_EQ(w.fetch_time, v.fetch_time);
  EXPECT_EQ(w.resolution_ms, v.resolution_ms);
  EXPECT_EQ(w.gateway_source, v.gateway_source);
  EXPECT_EQ(w.value, v.value);
}
TEST(IpnsRecordTest, TooBig) {
  ipfs::Byte* p = nullptr;
  ipfs::Cid cid(
      "k51qzi5uqu5dm4tm0wt8srkg9h9suud4wuiwjimndrkydqm81cqtlb5ak6p7ku"sv);
  EXPECT_TRUE(cid.hash_type() == i::HashType::IDENTITY);
  EXPECT_TRUE(cid.valid());
  Api api;
  LogRecorder lr;
  auto actual = ipfs::ValidateIpnsRecord({p, 12345}, cid, api);
  EXPECT_FALSE(actual.has_value());
}
TEST(IpnsRecordTest, copyctorcopiesfields) {
  ipfs::ValidatedIpns a;
  a.value = "val";
  a.gateway_source = "gw";
  a.sequence = 0;
  a.resolution_ms = 1;
  a.fetch_time = 2;
  a.cache_until = 3;
  a.use_until = 4;
  auto b = a;
  EXPECT_EQ(b.value, "val");
  EXPECT_EQ(b.gateway_source, "gw");
  auto i = 0;
  EXPECT_EQ(b.sequence, i++);
  EXPECT_EQ(b.resolution_ms, i++);
  EXPECT_EQ(b.fetch_time, i++);
  EXPECT_EQ(b.cache_until, i++);
  EXPECT_EQ(b.use_until, i++);
}
TEST(IpnsRecordTest, fromcborentry) {
  auto e = ipfs::IpnsCborEntry{"v", "2023-11-05T15:06:39.790199045Z", 0, 1, 99};
  auto now = std::time(nullptr);
  ipfs::ValidatedIpns v{e};
  EXPECT_EQ(v.value, "v");
  EXPECT_EQ(v.sequence, 1);
  EXPECT_EQ(v.use_until, 1699196799);
  auto time_left = v.cache_until - now;
  EXPECT_GE(time_left, 1);
  EXPECT_LE(time_left, 3);
}
TEST(IpnsRecordTest, notAProtoBuf) {
  ipfs::Cid cid{"k51qzi5uqu5dm4tm0wt8srkg9h9suud4wuiwjimndrkydqm81cqtlb5ak6p7ku"sv};
  Api api;
  LogRecorder rec;
  auto result = ipfs::ValidateIpnsRecord(ipfs::as_bytes("Not A ProtoBuf!"sv), cid, api);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(1UL,rec.messages.size());
  auto m = rec.messages.at(0UL).message;
  EXPECT_TRUE(m.find("Failed")<m.size()) << m;
  EXPECT_TRUE(m.find("protobuf")<m.size()) << m;
}

#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
using j = nlohmann::json;
TEST(IpnsRecordTest, V2_Only_Lean) {
  std::array<std::uint8_t, 188> from_spec{
      0x42, 0x40, 0x6a, 0x76, 0x98, 0x58, 0x5e, 0x2b, 0x17, 0x07, 0x09, 0xd4,
      0x94, 0x7e, 0x14, 0x8b, 0x18, 0xb1, 0x20, 0xd7, 0x6d, 0xb4, 0x35, 0x15,
      0xed, 0xac, 0xf2, 0x90, 0xdf, 0x96, 0xb7, 0x1e, 0x29, 0xc0, 0xca, 0x5a,
      0x16, 0x51, 0x02, 0x56, 0x81, 0x4f, 0x82, 0x55, 0x53, 0x2e, 0xc4, 0x54,
      0x9a, 0xde, 0x26, 0xf4, 0xca, 0x83, 0x35, 0xf4, 0xc4, 0x1e, 0x2c, 0xff,
      0xfe, 0xb1, 0xba, 0x0a, 0xa5, 0x01, 0x4a, 0x78, 0xa5, 0x63, 0x54, 0x54,
      0x4c, 0x1b, 0x00, 0x00, 0x01, 0xa3, 0x18, 0x5c, 0x50, 0x00, 0x65, 0x56,
      0x61, 0x6c, 0x75, 0x65, 0x58, 0x24, 0x2f, 0x69, 0x70, 0x66, 0x73, 0x2f,
      0x62, 0x61, 0x66, 0x6b, 0x71, 0x61, 0x64, 0x74, 0x77, 0x67, 0x69, 0x77,
      0x77, 0x36, 0x33, 0x74, 0x6d, 0x70, 0x65, 0x71, 0x68, 0x65, 0x7a, 0x6c,
      0x64, 0x6e, 0x35, 0x7a, 0x67, 0x69, 0x68, 0x53, 0x65, 0x71, 0x75, 0x65,
      0x6e, 0x63, 0x65, 0x00, 0x68, 0x56, 0x61, 0x6c, 0x69, 0x64, 0x69, 0x74,
      0x79, 0x58, 0x1b, 0x32, 0x31, 0x32, 0x33, 0x2d, 0x30, 0x38, 0x2d, 0x31,
      0x34, 0x54, 0x31, 0x32, 0x3a, 0x31, 0x37, 0x3a, 0x30, 0x33, 0x2e, 0x36,
      0x39, 0x34, 0x30, 0x35, 0x32, 0x5a, 0x6c, 0x56, 0x61, 0x6c, 0x69, 0x64,
      0x69, 0x74, 0x79, 0x54, 0x79, 0x70, 0x65, 0x00};
  auto* p = reinterpret_cast<std::byte const*>(from_spec.data());
  ipfs::ByteView as_seen_in_spec{p, from_spec.size()};
  ipfs::Cid cid(
      "k51qzi5uqu5dm4tm0wt8srkg9h9suud4wuiwjimndrkydqm81cqtlb5ak6p7ku"sv);
  EXPECT_TRUE(cid.valid());
  Api api{[](ipfs::ByteView c) {
    auto v = j::from_cbor(c);
    //        std::cout << std::setw(2) << v << std::endl;
    ipfs::IpnsCborEntry e;
    e.sequence = v.at("Sequence");
    auto& bin_val = v.at("Value").get_binary();
    e.value.assign(bin_val.begin(), bin_val.end());
    return e;
  }};
  auto cbor = std::make_unique<MockCbor>();

  api.cbors.push_back(std::move(cbor));
  MockApi api2;
  auto actual = ipfs::ValidateIpnsRecord(as_seen_in_spec, cid, api2);
  EXPECT_TRUE(actual);
}
TEST(IpnsRecordTest, has_pubkey_field) {
  std::array<std::uint8_t, 1059> dump_from_cli{
      0x0a, 0x34, 0x2f, 0x69, 0x70, 0x66, 0x73, 0x2f, 0x51, 0x6d, 0x56, 0x73,
      0x39, 0x58, 0x63, 0x48, 0x44, 0x7a, 0x58, 0x65, 0x48, 0x6d, 0x7a, 0x73,
      0x6d, 0x48, 0x47, 0x37, 0x33, 0x54, 0x54, 0x67, 0x6f, 0x66, 0x53, 0x71,
      0x42, 0x51, 0x70, 0x53, 0x54, 0x52, 0x52, 0x48, 0x33, 0x36, 0x44, 0x69,
      0x68, 0x67, 0x62, 0x59, 0x66, 0x34, 0x12, 0x80, 0x02, 0x4f, 0x8f, 0xd2,
      0x62, 0x29, 0x25, 0x01, 0x91, 0x04, 0xc9, 0xde, 0xb3, 0x50, 0x40, 0x78,
      0xc2, 0x7e, 0x62, 0xa8, 0xce, 0x4f, 0x21, 0xb7, 0x8d, 0xdf, 0x7e, 0x0c,
      0xda, 0xef, 0x13, 0xb3, 0x64, 0xa6, 0x5c, 0x32, 0xf3, 0x0a, 0x58, 0x44,
      0x48, 0x54, 0xde, 0xd5, 0x0a, 0x6a, 0x3f, 0xf1, 0xc1, 0xbf, 0x2d, 0xe8,
      0x90, 0x37, 0xbe, 0xd3, 0x43, 0xe0, 0x56, 0xcf, 0x99, 0x45, 0x2a, 0xaa,
      0x64, 0x3c, 0x34, 0xed, 0xf5, 0x36, 0x3f, 0x30, 0xee, 0xdb, 0xd7, 0xfd,
      0xc6, 0x31, 0x08, 0x6b, 0x9c, 0x62, 0x18, 0xa3, 0xd2, 0x61, 0x60, 0xe5,
      0x3c, 0xfd, 0xdc, 0x2e, 0xb9, 0x61, 0xe8, 0xa0, 0x0e, 0xa6, 0xc9, 0x73,
      0x3d, 0x68, 0xf6, 0xdf, 0x6b, 0xee, 0x4f, 0xed, 0x4f, 0x69, 0xce, 0x0f,
      0x52, 0x29, 0x97, 0x75, 0x04, 0x27, 0xf1, 0x5d, 0x06, 0x5c, 0x6d, 0x09,
      0xaa, 0xde, 0xea, 0xd9, 0x01, 0x87, 0x31, 0x3f, 0x84, 0xad, 0x7f, 0xb6,
      0x5b, 0xb3, 0x02, 0x5d, 0x49, 0x5a, 0x2e, 0x52, 0x11, 0xd4, 0x51, 0x8c,
      0x66, 0xa5, 0x41, 0xba, 0xc3, 0x71, 0x0d, 0x9e, 0x67, 0xd5, 0xa9, 0x53,
      0x07, 0x72, 0xb9, 0x95, 0xce, 0xe7, 0x72, 0x1a, 0xa6, 0xb5, 0xd4, 0xc9,
      0x23, 0x88, 0xb8, 0x47, 0x08, 0xb4, 0x7b, 0xb5, 0xfb, 0x3d, 0x59, 0xba,
      0x0c, 0x30, 0x06, 0xa8, 0xef, 0xd8, 0x14, 0x2f, 0x34, 0x0f, 0x94, 0x7b,
      0xb0, 0x82, 0x12, 0xa5, 0x1f, 0x7a, 0x3e, 0xdc, 0x58, 0x2c, 0xf4, 0x81,
      0x66, 0x2a, 0x51, 0x62, 0x44, 0x7a, 0xab, 0x5c, 0x8b, 0x91, 0x7e, 0x97,
      0xd6, 0x6a, 0xa4, 0xb8, 0x45, 0xbd, 0xe1, 0x1e, 0xba, 0xec, 0x1f, 0x90,
      0xf0, 0xc2, 0x47, 0xc7, 0xce, 0xd3, 0xfc, 0x5a, 0x55, 0xb1, 0xc9, 0x0f,
      0x7c, 0xd8, 0x86, 0x7d, 0xdb, 0x64, 0x21, 0x28, 0x81, 0x7f, 0x52, 0x09,
      0x40, 0x18, 0x00, 0x22, 0x1e, 0x32, 0x30, 0x32, 0x33, 0x2d, 0x31, 0x31,
      0x2d, 0x30, 0x37, 0x54, 0x31, 0x35, 0x3a, 0x30, 0x36, 0x3a, 0x33, 0x39,
      0x2e, 0x37, 0x39, 0x30, 0x31, 0x39, 0x39, 0x30, 0x34, 0x35, 0x5a, 0x28,
      0x00, 0x30, 0x80, 0xb0, 0x9d, 0xc2, 0xdf, 0x01, 0x3a, 0xab, 0x02, 0x08,
      0x00, 0x12, 0xa6, 0x02, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09,
      0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03,
      0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01,
      0x00, 0xb0, 0xe4, 0x02, 0xef, 0x68, 0xd9, 0x27, 0xe6, 0xe8, 0x66, 0xeb,
      0xd8, 0xcd, 0x24, 0xc5, 0x44, 0x58, 0x4c, 0x57, 0xac, 0x59, 0x4c, 0xe6,
      0x7a, 0x8a, 0x86, 0xc5, 0xb6, 0xef, 0x8d, 0x86, 0xbf, 0x8e, 0xa0, 0x49,
      0xd8, 0x20, 0x4c, 0x60, 0xf7, 0x88, 0xed, 0x26, 0xff, 0x01, 0xec, 0x8e,
      0x4b, 0x59, 0xa0, 0x6f, 0xdd, 0x65, 0x2b, 0x03, 0xbf, 0xae, 0x0b, 0xbe,
      0x12, 0xb9, 0xea, 0x83, 0x49, 0x0f, 0x6b, 0xb3, 0x1a, 0x5c, 0x19, 0x02,
      0x7a, 0x16, 0x9d, 0x2d, 0x23, 0x65, 0xb8, 0x01, 0xc1, 0x8b, 0x57, 0x14,
      0x55, 0xbe, 0xa6, 0x74, 0x4c, 0x78, 0xb5, 0xde, 0x92, 0x6c, 0x23, 0x7f,
      0x7e, 0xf0, 0xd8, 0xea, 0xf9, 0x0a, 0x5b, 0xbd, 0xc2, 0xca, 0xb6, 0x1c,
      0x08, 0xe6, 0xc5, 0x00, 0x9c, 0xc6, 0xbd, 0x14, 0x9e, 0x50, 0xd3, 0x43,
      0xb0, 0xb2, 0xf9, 0xb9, 0xd8, 0x45, 0xf9, 0x25, 0xbc, 0x62, 0x54, 0x13,
      0xa2, 0x6e, 0xb2, 0xbb, 0x76, 0xfa, 0x68, 0xcb, 0xe2, 0x2f, 0xb8, 0xd4,
      0x31, 0x99, 0xfc, 0x44, 0xa2, 0xcd, 0xea, 0xca, 0x30, 0xcb, 0xf6, 0x97,
      0xc5, 0x35, 0x16, 0xaa, 0xbc, 0x12, 0xb9, 0x84, 0xc6, 0x5a, 0x37, 0xb7,
      0x5f, 0x0b, 0x8e, 0x48, 0xf9, 0x15, 0xe4, 0xb6, 0xa5, 0x73, 0xce, 0xd9,
      0xa7, 0x9f, 0x7a, 0xff, 0xd8, 0x07, 0x86, 0x1b, 0xa3, 0xb7, 0xf2, 0xbc,
      0xdc, 0xad, 0xf3, 0x21, 0x7d, 0xcb, 0x4c, 0x08, 0xa2, 0xec, 0x68, 0x37,
      0xe1, 0xf9, 0xff, 0x2d, 0x6e, 0x7c, 0x19, 0xbd, 0xc1, 0x96, 0x46, 0xae,
      0xfe, 0x69, 0x00, 0xd1, 0x20, 0x8d, 0xe4, 0xca, 0x7c, 0x36, 0x80, 0xf3,
      0xfb, 0x05, 0xe4, 0x5e, 0xd0, 0xd7, 0x1e, 0x7e, 0x56, 0x69, 0x25, 0x81,
      0xa0, 0x2f, 0x78, 0x71, 0x9c, 0x85, 0x59, 0x42, 0xe2, 0x9b, 0x5a, 0xe2,
      0x3c, 0x48, 0x75, 0x8d, 0x91, 0x02, 0x03, 0x01, 0x00, 0x01, 0x42, 0x80,
      0x02, 0x3e, 0x34, 0x34, 0x9b, 0xa1, 0x0f, 0xb7, 0x7f, 0xbb, 0xbe, 0x88,
      0x81, 0x43, 0x96, 0x25, 0x13, 0x9b, 0x0a, 0x91, 0x56, 0xa9, 0xce, 0x0c,
      0xb6, 0x20, 0x78, 0xb6, 0xf8, 0x5e, 0xb3, 0xd8, 0x03, 0xab, 0x06, 0x2f,
      0xfe, 0x78, 0x07, 0xbf, 0x75, 0x23, 0x2a, 0x2b, 0xaf, 0x23, 0x7a, 0xd4,
      0xec, 0xba, 0x9d, 0xa6, 0x71, 0x7d, 0x90, 0xbb, 0x4c, 0x6e, 0xc0, 0x13,
      0x38, 0xc8, 0x0d, 0x0d, 0x89, 0x71, 0xab, 0xb3, 0x5c, 0x16, 0x94, 0x6a,
      0xea, 0xda, 0xc1, 0x0b, 0xa6, 0x6d, 0x5b, 0x41, 0x7a, 0xf4, 0x7e, 0xaf,
      0x0f, 0xe9, 0xfc, 0x40, 0x7f, 0xff, 0xd6, 0x7d, 0x15, 0xef, 0x8f, 0x9f,
      0xaa, 0xe2, 0x31, 0x3a, 0x49, 0x24, 0x39, 0xc8, 0xdb, 0x8e, 0xfb, 0xcc,
      0x3e, 0xb2, 0x0d, 0x7e, 0xd0, 0x84, 0xdc, 0x98, 0x08, 0xe8, 0xa3, 0x76,
      0x44, 0xb6, 0xe0, 0xee, 0xe2, 0xde, 0xde, 0x07, 0xe6, 0x7a, 0xf0, 0xee,
      0xda, 0x00, 0x77, 0xae, 0x96, 0x7c, 0xfb, 0xdb, 0x0a, 0xa0, 0x8b, 0xdd,
      0xb1, 0x0b, 0x48, 0x56, 0xcf, 0x35, 0x9c, 0xfc, 0x21, 0x7e, 0x73, 0xc3,
      0xa6, 0x41, 0x8d, 0x5f, 0xe7, 0x39, 0xe9, 0x0d, 0x65, 0xe8, 0x06, 0xcf,
      0xb4, 0xce, 0x3c, 0xe1, 0x5e, 0xa0, 0xa1, 0x66, 0xd0, 0xc8, 0xd9, 0x27,
      0x48, 0x46, 0x40, 0x09, 0xbf, 0x06, 0xae, 0x06, 0x74, 0x61, 0x3a, 0x8b,
      0x6e, 0xaf, 0xdf, 0x15, 0xb7, 0x61, 0xc0, 0xb6, 0x7d, 0x04, 0xff, 0x79,
      0x69, 0x2f, 0xf6, 0xe3, 0x1b, 0xde, 0x2c, 0xb8, 0x4b, 0x6c, 0x58, 0x73,
      0x07, 0x65, 0xed, 0x2c, 0xe8, 0x38, 0x50, 0x67, 0x87, 0x2b, 0x2a, 0x36,
      0x01, 0x64, 0x74, 0x55, 0xe8, 0xf4, 0xad, 0x8f, 0x7d, 0x49, 0xcb, 0xf1,
      0x8f, 0x66, 0xd3, 0xc1, 0xf3, 0xad, 0xb9, 0x67, 0xb1, 0x15, 0x89, 0x42,
      0x78, 0x61, 0x4b, 0xfe, 0x85, 0x4a, 0x8b, 0x01, 0xa5, 0x63, 0x54, 0x54,
      0x4c, 0x1b, 0x00, 0x00, 0x00, 0x0d, 0xf8, 0x47, 0x58, 0x00, 0x65, 0x56,
      0x61, 0x6c, 0x75, 0x65, 0x58, 0x34, 0x2f, 0x69, 0x70, 0x66, 0x73, 0x2f,
      0x51, 0x6d, 0x56, 0x73, 0x39, 0x58, 0x63, 0x48, 0x44, 0x7a, 0x58, 0x65,
      0x48, 0x6d, 0x7a, 0x73, 0x6d, 0x48, 0x47, 0x37, 0x33, 0x54, 0x54, 0x67,
      0x6f, 0x66, 0x53, 0x71, 0x42, 0x51, 0x70, 0x53, 0x54, 0x52, 0x52, 0x48,
      0x33, 0x36, 0x44, 0x69, 0x68, 0x67, 0x62, 0x59, 0x66, 0x34, 0x68, 0x53,
      0x65, 0x71, 0x75, 0x65, 0x6e, 0x63, 0x65, 0x00, 0x68, 0x56, 0x61, 0x6c,
      0x69, 0x64, 0x69, 0x74, 0x79, 0x58, 0x1e, 0x32, 0x30, 0x32, 0x33, 0x2d,
      0x31, 0x31, 0x2d, 0x30, 0x37, 0x54, 0x31, 0x35, 0x3a, 0x30, 0x36, 0x3a,
      0x33, 0x39, 0x2e, 0x37, 0x39, 0x30, 0x31, 0x39, 0x39, 0x30, 0x34, 0x35,
      0x5a, 0x6c, 0x56, 0x61, 0x6c, 0x69, 0x64, 0x69, 0x74, 0x79, 0x54, 0x79,
      0x70, 0x65, 0x00

  };
  auto* p = reinterpret_cast<std::byte const*>(dump_from_cli.data());
  ipfs::ByteView as_seen_in_spec{p, dump_from_cli.size()};
  auto c =
      ipfs::Cid{"k2k4r8p8axjle4tulaj8f6423nuwnyjswl3iq7ppmqci5efgn7vg6ah4"};
  EXPECT_EQ(c.codec(), ipfs::MultiCodec::LIBP2P_KEY);
  EXPECT_EQ(c.hash_type(), ipfs::HashType::SHA2_256);
  EXPECT_TRUE(c.valid());
  Api api{[](ipfs::ByteView c) {
    auto v = j::from_cbor(c);
    //        std::cout << std::setw(2) << v << std::endl;
    ipfs::IpnsCborEntry e;
    e.sequence = v.at("Sequence");
    auto& bin_val = v.at("Value").get_binary();
    e.value.assign(bin_val.begin(), bin_val.end());
    auto& bin_vdty = v.at("Validity").get_binary();
    e.validity.assign(bin_vdty.begin(), bin_vdty.end());
    e.ttl = v.at("TTL");
    e.validityType = v.at("ValidityType");
    return e;
  }};
  MockCbor cbor;
  cbor.set("Value", "/ipfs/QmVs9XcHDzXeHmzsmHG73TTgofSqBQpSTRRH36DihgbYf4"s);
  cbor.set("Validity", "2023-11-07T15:06:39.790199045Z"s);
  cbor.set("TTL", 60000000000UL);
  cbor.set("Sequence", 0UL);
  cbor.set("ValidityType", 0UL);
  api.cbors.push_back(std::make_unique<MockCbor>(cbor));
  auto actual = ipfs::ValidateIpnsRecord(as_seen_in_spec, c, api);
  EXPECT_TRUE(actual);
}
TEST(IpnsRecordTest, ValueMismatch) {
  std::array<std::uint8_t,398> xxd{
    0x0a, 0x41, 0x2f, 0x69, 0x70, 0x66, 0x73, 0x2f, 0x62, 0x61, 0x66, 0x79,
    0x62, 0x65, 0x69, 0x62, 0x37, 0x72, 0x7a, 0x71, 0x70, 0x6b, 0x65, 0x75,
    0x69, 0x6d, 0x61, 0x64, 0x72, 0x65, 0x66, 0x73, 0x6f, 0x63, 0x64, 0x66,
    0x76, 0x79, 0x6e, 0x36, 0x32, 0x37, 0x6b, 0x33, 0x6a, 0x76, 0x61, 0x63,
    0x77, 0x75, 0x32, 0x70, 0x6d, 0x66, 0x75, 0x70, 0x6a, 0x6e, 0x79, 0x74,
    0x33, 0x67, 0x6f, 0x6e, 0x70, 0x32, 0x75, 0x12, 0x40, 0x7f, 0x0d, 0x5e,
    0x0d, 0x1b, 0xf9, 0x3f, 0x9b, 0x85, 0x98, 0xe5, 0xcf, 0xda, 0xc4, 0x34,
    0xed, 0x17, 0x7a, 0xa8, 0x61, 0xdc, 0xef, 0x7e, 0x76, 0x67, 0x06, 0x0e,
    0x1f, 0x86, 0xe7, 0x1d, 0x3b, 0x91, 0x25, 0x30, 0x37, 0xb8, 0xb9, 0xae,
    0x19, 0x41, 0xc8, 0x27, 0xe5, 0x88, 0xdb, 0x6a, 0x2d, 0x60, 0xf2, 0xca,
    0x52, 0x57, 0x2b, 0x57, 0xc4, 0x78, 0xa8, 0x78, 0xcc, 0x71, 0xe6, 0xe2,
    0x04, 0x18, 0x00, 0x22, 0x1e, 0x32, 0x30, 0x32, 0x34, 0x2d, 0x30, 0x38,
    0x2d, 0x32, 0x32, 0x54, 0x31, 0x34, 0x3a, 0x31, 0x31, 0x3a, 0x31, 0x34,
    0x2e, 0x37, 0x35, 0x35, 0x33, 0x37, 0x36, 0x39, 0x30, 0x36, 0x5a, 0x28,
    0x48, 0x30, 0x80, 0xc0, 0xe2, 0x85, 0xe3, 0x68, 0x42, 0x40, 0xec, 0x09,
    0xc1, 0x97, 0xf6, 0xb9, 0xc8, 0x39, 0x72, 0x06, 0x4f, 0x33, 0x3b, 0x1c,
    0x6c, 0x43, 0x6d, 0x35, 0x4b, 0x10, 0xd1, 0x4b, 0x77, 0x4e, 0xa2, 0xd6,
    0xe5, 0xcc, 0x6e, 0x76, 0x45, 0x1c, 0xaa, 0x74, 0xa1, 0x91, 0xb1, 0xd7,
    0xa9, 0x58, 0xbb, 0x59, 0x59, 0x33, 0x63, 0x11, 0xc5, 0xfb, 0x03, 0x9c,
    0xe5, 0x39, 0x88, 0x33, 0xd5, 0x34, 0xbf, 0x26, 0xe6, 0x90, 0x60, 0x86,
    0xf2, 0x0c, 0x4a, 0x99, 0x01, 0xa5, 0x63, 0x54, 0x54, 0x4c, 0x1b, 0x00,
    0x00, 0x03, 0x46, 0x30, 0xb8, 0xa0, 0x00, 0x65, 0x56, 0x61, 0x6c, 0x75,
    0x65, 0x58, 0x41, 0x2f, 0x69, 0x70, 0x66, 0x73, 0x2f, 0x62, 0x61, 0x66,
    0x79, 0x62, 0x65, 0x69, 0x62, 0x37, 0x72, 0x7a, 0x71, 0x70, 0x6b, 0x65,
    0x75, 0x69, 0x6d, 0x61, 0x64, 0x72, 0x65, 0x66, 0x73, 0x6f, 0x63, 0x64,
    0x66, 0x76, 0x79, 0x6e, 0x36, 0x32, 0x37, 0x6b, 0x33, 0x6a, 0x76, 0x61,
    0x63, 0x77, 0x75, 0x32, 0x70, 0x6d, 0x66, 0x75, 0x70, 0x6a, 0x6e, 0x79,
    0x74, 0x33, 0x67, 0x6f, 0x6e, 0x70, 0x32, 0x75, 0x68, 0x53, 0x65, 0x71,
    0x75, 0x65, 0x6e, 0x63, 0x65, 0x18, 0x48, 0x68, 0x56, 0x61, 0x6c, 0x69,
    0x64, 0x69, 0x74, 0x79, 0x58, 0x1e, 0x32, 0x30, 0x32, 0x34, 0x2d, 0x30,
    0x38, 0x2d, 0x32, 0x32, 0x54, 0x31, 0x34, 0x3a, 0x31, 0x31, 0x3a, 0x31,
    0x34, 0x2e, 0x37, 0x35, 0x35, 0x33, 0x37, 0x36, 0x39, 0x30, 0x36, 0x5a,
    0x6c, 0x56, 0x61, 0x6c, 0x69, 0x64, 0x69, 0x74, 0x79, 0x54, 0x79, 0x70,
    0x65, 0x00
    };
  EXPECT_EQ(xxd[8], 0x62);//b
  EXPECT_EQ(xxd[9], 0x61);//a
  EXPECT_EQ(xxd[10], 0x66);//f
  EXPECT_EQ(xxd[11], 0x79);//y
  EXPECT_EQ(xxd[12], 0x62);//b
  i::Cid cid{"k51qzi5uqu5dkq4jxcqvujfm2woh4p9y6inrojofxflzdnfht168zf8ynfzuu1"};
  Api api{[](ipfs::ByteView c) {
    auto v = j::from_cbor(c);
    //        std::cout << std::setw(2) << v << std::endl;
    ipfs::IpnsCborEntry e;
    e.sequence = v.at("Sequence");
    auto& bin_val = v.at("Value").get_binary();
    e.value.assign(bin_val.begin(), bin_val.end());
    return e;
  }};
  auto real_validates = ipfs::ValidateIpnsRecord(i::as_bytes(xxd), cid, api);
  EXPECT_TRUE(real_validates);
  xxd[12]++;//cause a mismatch
  il::SetLevel(il::Level::Error);
  il::SetHandler(save_log);
  auto modified_validates = ipfs::ValidateIpnsRecord(i::as_bytes(xxd), cid, api);
  EXPECT_FALSE(modified_validates) << all_log;
  il::SetHandler(il::DefaultHandler);
  EXPECT_EQ(log_lines.size(),1UL) << all_log;
  auto& log_line = log_lines.at(0);
  EXPECT_LT(log_line.find("Mismatch on Value"),log_line.size()) << all_log;
}
#endif
