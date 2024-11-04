#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/partition.h>

#include <mock_api.h>
#include <log_recorder.h>
#include <mock_requestor.h>
#include "ipfs_client/gw/gateway_request_type.h"

using T = ig::GatewayRequest;

namespace {
constexpr std::array<std::uint8_t const, 491> a_car{
    0x0a, 0xa1, 0x67, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x02, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3a, 0xa2, 0x65, 0x72, 0x6f, 0x6f, 0x74, 0x73, 0x81,
    0xd8, 0x2a, 0x58, 0x25, 0x00, 0x01, 0x70, 0x12, 0x20, 0x36, 0x28, 0x15,
    0x89, 0x64, 0x53, 0x16, 0x7b, 0xc1, 0xd1, 0xa6, 0x80, 0x31, 0xf4, 0x80,
    0x9b, 0xe8, 0x0c, 0x94, 0x9d, 0xca, 0x5c, 0xeb, 0x59, 0x85, 0xce, 0x93,
    0xcc, 0xef, 0x34, 0x0f, 0xbe, 0x67, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f,
    0x6e, 0x01, 0x28, 0x01, 0x55, 0x12, 0x20, 0x77, 0x0e, 0x60, 0x76, 0x24,
    0xd6, 0x89, 0x26, 0x5c, 0xa6, 0xc4, 0x48, 0x84, 0xd0, 0x80, 0x7d, 0x9b,
    0x05, 0x4d, 0x23, 0xc4, 0x73, 0xc1, 0x06, 0xc7, 0x2b, 0xe9, 0xde, 0x08,
    0xb7, 0x37, 0x6c, 0x67, 0x6f, 0x6f, 0x64, 0x32, 0x01, 0x55, 0x12, 0x20,
    0xd2, 0x69, 0x9a, 0x29, 0x4b, 0xbd, 0x90, 0x4d, 0x1d, 0xff, 0x27, 0x48,
    0x9b, 0xef, 0x29, 0x26, 0x77, 0x94, 0xf4, 0xab, 0xe2, 0x63, 0x91, 0x0d,
    0xbe, 0x8f, 0x76, 0xba, 0xb0, 0xe7, 0x2b, 0x19, 0x48, 0x65, 0x6c, 0x6c,
    0x6f, 0x20, 0x57, 0x68, 0x69, 0x72, 0x6c, 0x65, 0x64, 0x21, 0x89, 0x01,
    0x01, 0x70, 0x12, 0x20, 0x36, 0x28, 0x15, 0x89, 0x64, 0x53, 0x16, 0x7b,
    0xc1, 0xd1, 0xa6, 0x80, 0x31, 0xf4, 0x80, 0x9b, 0xe8, 0x0c, 0x94, 0x9d,
    0xca, 0x5c, 0xeb, 0x59, 0x85, 0xce, 0x93, 0xcc, 0xef, 0x34, 0x0f, 0xbe,
    0x12, 0x2e, 0x0a, 0x24, 0x01, 0x55, 0x12, 0x20, 0x77, 0x0e, 0x60, 0x76,
    0x24, 0xd6, 0x89, 0x26, 0x5c, 0xa6, 0xc4, 0x48, 0x84, 0xd0, 0x80, 0x7d,
    0x9b, 0x05, 0x4d, 0x23, 0xc4, 0x73, 0xc1, 0x06, 0xc7, 0x2b, 0xe9, 0xde,
    0x08, 0xb7, 0x37, 0x6c, 0x12, 0x04, 0x67, 0x6f, 0x6f, 0x64, 0x18, 0x04,
    0x12, 0x2f, 0x0a, 0x24, 0x01, 0x55, 0x12, 0x20, 0xd2, 0x69, 0x9a, 0x29,
    0x4b, 0xbd, 0x90, 0x4d, 0x1d, 0xff, 0x27, 0x48, 0x9b, 0xef, 0x29, 0x26,
    0x77, 0x94, 0xf4, 0xab, 0xe2, 0x63, 0x91, 0x0d, 0xbe, 0x8f, 0x76, 0xba,
    0xb0, 0xe7, 0x2b, 0x19, 0x12, 0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x18,
    0x0e, 0x0a, 0x02, 0x08, 0x01, 0x81, 0x08, 0x01, 0x00, 0x00, 0x00, 0x12,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x28,
    0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36,
    0x28, 0x15, 0x89, 0x64, 0x53, 0x16, 0x7b, 0xc1, 0xd1, 0xa6, 0x80, 0x31,
    0xf4, 0x80, 0x9b, 0xe8, 0x0c, 0x94, 0x9d, 0xca, 0x5c, 0xeb, 0x59, 0x85,
    0xce, 0x93, 0xcc, 0xef, 0x34, 0x0f, 0xbe, 0x97, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x77, 0x0e, 0x60, 0x76, 0x24, 0xd6, 0x89, 0x26, 0x5c,
    0xa6, 0xc4, 0x48, 0x84, 0xd0, 0x80, 0x7d, 0x9b, 0x05, 0x4d, 0x23, 0xc4,
    0x73, 0xc1, 0x06, 0xc7, 0x2b, 0xe9, 0xde, 0x08, 0xb7, 0x37, 0x6c, 0x3b,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd2, 0x69, 0x9a, 0x29, 0x4b,
    0xbd, 0x90, 0x4d, 0x1d, 0xff, 0x27, 0x48, 0x9b, 0xef, 0x29, 0x26, 0x77,
    0x94, 0xf4, 0xab, 0xe2, 0x63, 0x91, 0x0d, 0xbe, 0x8f, 0x76, 0xba, 0xb0,
    0xe7, 0x2b, 0x19, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
struct GatewayRequestTest : public testing::Test {
  T t_;
  std::shared_ptr<MockApi> api = std::make_shared<MockApi>();
  std::shared_ptr<MockRequestor> rtor = std::make_shared<MockRequestor>();
  std::shared_ptr<i::Partition> orc;
  GatewayRequestTest() {
    orc = api->with(rtor).partition({});
    t_.orchestrator(orc);
    t_.root_component("main");
  }
  short timeout(RT t) {
   T gr;
   gr.type = t;
   return gr.timeout_seconds();
  }
};
}  // namespace

TEST_F(GatewayRequestTest, RespondsToCar) {
  LogRecorder lr;
  t_.cid(i::Cid{"bafybeibwfakyszctcz54dungqay7jae35agjjhokltvvtboospgo6napxy"});
  t_.type = RT::Car;
  auto p = reinterpret_cast<char const*>(a_car.data());
  EXPECT_FALSE(orc->has_key(
      "bafkreidxbzqhmjgwretfzjwejccnbad5tmcu2i6eopaqnrzl5hparnzxnq"));
  EXPECT_FALSE(orc->has_key(
      "bafkreigsngncss55sbgr37zhjcn66kjgo6kpjk7cmoiq3pupo25lbzzlde"));
  EXPECT_FALSE(orc->has_key(
      "bafybeibwfakyszctcz54dungqay7jae35agjjhokltvvtboospgo6napxy"));
  t_.RespondSuccessfully({p, a_car.size()}, api, {});
  EXPECT_TRUE(orc->has_key(
      "bafkreidxbzqhmjgwretfzjwejccnbad5tmcu2i6eopaqnrzl5hparnzxnq"));
  EXPECT_TRUE(orc->has_key(
      "bafkreigsngncss55sbgr37zhjcn66kjgo6kpjk7cmoiq3pupo25lbzzlde"));
  EXPECT_TRUE(orc->has_key(
      "bafybeibwfakyszctcz54dungqay7jae35agjjhokltvvtboospgo6napxy"));
}
TEST_F(GatewayRequestTest, suffices) {
  t_.type = RT::Car;
  EXPECT_EQ(t_.url_suffix(), "/ipfs/main/?entity-bytes=0:3495253");
  t_.type = RT::Ipns;
  EXPECT_EQ(t_.url_suffix(), "/ipns/main");
  t_.type = RT::DnsLink;
  EXPECT_EQ(t_.url_suffix(), "/ipns/main");
  t_.type = RT::Providers;
  EXPECT_EQ(t_.url_suffix(), "/routing/v1/providers/main");
  t_.type = RT::Identity;
  EXPECT_EQ(t_.url_suffix(), "");
  t_.type = RT::Zombie;
  EXPECT_EQ(t_.url_suffix(), "");
}
TEST_F(GatewayRequestTest, accept_param) {
  t_.type = RT::Car;
  EXPECT_EQ(t_.accept(), "application/vnd.ipld.car");
  t_.type = RT::Ipns;
  EXPECT_EQ(t_.accept(), "application/vnd.ipfs.ipns-record");
  t_.type = RT::Providers;
  EXPECT_EQ(t_.accept(), "application/json");
  t_.type = RT::Identity;
  EXPECT_EQ(t_.accept(), "");
  t_.type = RT::Zombie;
  EXPECT_EQ(t_.accept(), "");
  t_.type = RT::DnsLink;
  EXPECT_EQ(t_.accept(), "application/vnd.ipld.raw");
}
TEST_F(GatewayRequestTest, timeouts_ordinal) {
 EXPECT_LT(timeout(RT::Identity), timeout(RT::DnsLink));
 EXPECT_LT(timeout(RT::Providers), timeout(RT::Block));
 EXPECT_LT(timeout(RT::Car), timeout(RT::Ipns));
}
TEST_F(GatewayRequestTest, describe_block_http) {
 t_.type = RT::Block;
 auto o = t_.describe_http("http://gate.way/");
 ASSERT_TRUE(o.has_value());
 auto& d = *o;
 EXPECT_EQ(d.url, "http://gate.way/ipfs/main");
 EXPECT_EQ(d.accept, "application/vnd.ipld.raw");
 EXPECT_EQ(d.timeout_seconds, 32);
 ASSERT_TRUE(d.max_response_size.has_value());
 EXPECT_EQ(d.max_response_size.value(), 2097152);
}
TEST_F(GatewayRequestTest, from_path_unsupportedCID) {
  i::SlashDelimited p{
    "/ipfs/bafkrsqgi3m3fi6kqwdzbucyn3fo4tv7pq5hqig7npu5o7gdcmk45a7chshayimpzumfakl732bqxmtata6y2jf43rm6z3bc3vxm7vzyss2366"
  };
  //ipfs_client doesn't support shake-256 by default
  auto t = T::fromIpfsPath(p);
  EXPECT_FALSE(t);
}
TEST_F(GatewayRequestTest, from_path_detectsInline) {
  i::SlashDelimited p{
      "/ipfs/bafyaacqkbaeaeeqcnburqaq"
  };
  //ipfs_client doesn't support shake-256 by default
  auto t = T::fromIpfsPath(p);
  ASSERT_TRUE(t);
  EXPECT_TRUE(t->type==RT::Identity);
}
