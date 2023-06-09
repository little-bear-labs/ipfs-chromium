#include "ipfs_client/scheduler.h"

#include <ipfs_client/context_api.h>

#include <gtest/gtest.h>

namespace {
ipfs::GatewayList gen() {
  return {};
}
struct SchedulerTest : public ::testing::Test {
  ipfs::Scheduler under_test_;
  SchedulerTest() : under_test_(&gen) {}
};
}  // namespace

TEST_F(SchedulerTest, EmptyNewSchedulerIsNotOverburdened) {
  EXPECT_TRUE(under_test_.IssueRequests({}));
}
TEST_F(SchedulerTest, EmptySchedulerIsAlwaysCompleteFailureForAnything) {
  EXPECT_TRUE(under_test_.DetectCompleteFailure("No such task"));
}
