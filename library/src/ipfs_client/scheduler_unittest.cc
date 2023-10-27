#include "ipfs_client/scheduler.h"

#include <ipfs_client/context_api.h>
#include <ipfs_client/logger.h>

#include <gtest/gtest.h>

namespace {
ipfs::GatewayList gen() {
  return {};
}
std::vector<std::pair<std::string, ipfs::log::Level>> logmsgs;
void SaveLog(std::string const& message,
             char const*,
             int,
             ipfs::log::Level for_prefix) {
  std::cout << "SAVING log message " << message << std::endl;
  logmsgs.push_back({message, for_prefix});
}
struct SchedulerTest : public ::testing::Test {
  ipfs::Scheduler under_test_;
  SchedulerTest() : under_test_(&gen) {
    logmsgs.clear();
    ipfs::log::SetHandler(&SaveLog);
    //    std::clog << "ctor" << std::endl;
  }
  ~SchedulerTest() {
    //    std::clog << "dtor" << std::endl;
    ipfs::log::SetHandler(&ipfs::log::DefaultHandler);
    logmsgs.clear();
  }
};
}  // namespace

TEST_F(SchedulerTest, EmptyNewSchedulerIsNotOverburdened) {
  EXPECT_TRUE(under_test_.IssueRequests({}));
}
TEST_F(SchedulerTest, EmptySchedulerIsAlwaysCompleteFailureForAnything) {
  EXPECT_TRUE(under_test_.DetectCompleteFailure({"No such task", ""}));
}
TEST_F(SchedulerTest, NonExistentTaskCompletingIsHarmless) {
  //If we wanted to we could assert a log line is written at verbose level
  EXPECT_NO_THROW(under_test_.TaskComplete({"No such task", ""}));
}
/*
TEST_F(SchedulerTest, NoSuffixErrorMessage) {
  under_test_.Enqueue({}, {}, {}, {}, {});
  EXPECT_GE(logmsgs.size(), 1UL);
  EXPECT_EQ(logmsgs.at(0).second, ipfs::log::Level::ERROR);
  EXPECT_LT(logmsgs.at(0).first.find("suffix"), 99);
}
*/
