#include <ipfs_client/gw/dnslink_requestor.h>

#include <mock_api.h>
#include <log_recorder.h>

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/gw/terminating_requestor.h>
#include <ipfs_client/partition.h>

using T = ig::DnsLinkRequestor;

namespace {
struct DnslinkRequestorTest : public testing::Test {
  LogRecorder logs_;
  std::shared_ptr<MockApi> api = std::make_shared<MockApi>();
  std::shared_ptr<T> t_ = std::make_shared<T>(api);
  std::shared_ptr<ig::GatewayRequest> r_ =
      ig::GatewayRequest::fromIpfsPath(i::SlashDelimited{"/ipns/not_a_cid"});
  ig::Requestor& t() { return *t_; }
  std::shared_ptr<ig::Requestor> rtor_;
  std::shared_ptr<i::Partition> orc_;
  DnslinkRequestorTest() {
    rtor_ = std::make_shared<ig::TerminatingRequestor>();
    orc_ = api->with(rtor_).partition({});
    r_->orchestrator(orc_);
  }
};
}  // namespace

TEST_F(DnslinkRequestorTest, basic) {
  api->dns_->expected_dns.push_back({"_dnslink.not_a_cid",
                                     {"dnslinks=/ipfs/nonotthis"s,
                                "dnslink=/ipfs/yesthis"s
                                ""s}});
  EXPECT_FALSE(orc_->has_key("not_a_cid"));
  auto res = t_->handle(r_);
  EXPECT_EQ(static_cast<char>(res), 'P');
  EXPECT_TRUE(orc_->has_key("not_a_cid"));
}
