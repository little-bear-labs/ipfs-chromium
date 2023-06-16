#include <ipfs_client/context_api.h>

#include <gtest/gtest.h>

namespace ipfs {
class BusyGateway;
class Scheduler;
}

namespace {
struct TestParams {
  std::string gw_pre = "gw_pre/";
  std::string tsk_suf = "tsk_suf";
  ipfs::Scheduler* sched = nullptr;
  std::unique_ptr<ipfs::BusyGateway> under_test;//name borrowed, not main target of tets here
};

struct ContextApiTest : public ::testing::Test {
  TestParams params;
  std::unique_ptr<ipfs::GatewayRequest> gr;
  ContextApiTest() {
    ipfs::BusyGateway::TestAccess(&params);
    gr = std::make_unique<ipfs::GatewayRequest>(std::move(*params.under_test));
    params.under_test.reset();
  }
};
}

TEST_F(ContextApiTest,url_is_passthrough) {
  EXPECT_EQ("gw_pre/tsk_suf", gr->url());
}
TEST_F(ContextApiTest,task_is_passthrough) {
  EXPECT_EQ("tsk_suf", gr->task());
}

void ipfs::BusyGateway::TestAccess(TestParams* p) {
  p->under_test.reset(new BusyGateway(p->gw_pre, p->tsk_suf, p->sched));
}
