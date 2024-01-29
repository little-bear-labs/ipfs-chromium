#include <ipfs_client/client.h>

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
};

struct ContextApiTest : public ::testing::Test {
  TestParams params;
  ContextApiTest() {
  }
};
}
