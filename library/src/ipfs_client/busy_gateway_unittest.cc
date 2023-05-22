#include <ipfs_client/busy_gateway.h>

#include <gtest/gtest.h>

#include <ipfs_client/scheduler.h>

using ipfs::BusyGateway;
using ipfs::Gateway;
using ipfs::GatewayList;
using ipfs::Scheduler;

namespace {

GatewayList gateway_list() {
  return {Gateway{"gw_pre", 9}};
}
struct TestParams {
  std::string gw_pre = "gw_pre";
  std::string tsk_suf = "tsk_suf";
  Scheduler* sched = nullptr;
  std::unique_ptr<BusyGateway> under_test;
};

struct BusyGatewayTest : public testing::Test {
  Scheduler sched;
  TestParams params;

  BusyGatewayTest() : sched(gateway_list) {
    params.sched = &sched;
    BusyGateway::TestAccess(&params);
  }

  BusyGateway& under_test() {
    auto* p = params.under_test.get();
    EXPECT_TRUE(!!p);
    return *p;
  }
};
}  // namespace

TEST_F(BusyGatewayTest, get_Successful) {
  auto* actual = under_test().get();
  ASSERT_TRUE(actual);
  EXPECT_EQ(actual->url_prefix(), "gw_pre");
}

void BusyGateway::TestAccess(TestParams* p) {
  p->under_test.reset(new BusyGateway(p->gw_pre, p->tsk_suf, p->sched));
}
