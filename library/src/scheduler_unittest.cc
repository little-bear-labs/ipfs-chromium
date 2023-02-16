#include "ipfs_client/scheduler.h"

#include <gtest/gtest.h>

TEST(IpfsSchedulerTest, AssignBusyGateway) {
  ipfs::GatewayList gws{ipfs::Gateway{"https://example.com/", 99}};
  std::vector<ipfs::BusyGateway> bgws;
  auto requestor = [&bgws](ipfs::BusyGateway g) {
    bgws.emplace_back(std::move(g));
  };
  ipfs::Scheduler sched(std::move(gws), requestor);
  sched.Enqueue("ipfs/CoolStuff", ipfs::Scheduler::Priority::Required);
  EXPECT_EQ(bgws.size(), 1UL);
  EXPECT_TRUE(bgws.at(0));
  EXPECT_EQ(bgws.at(0)->url(), "https://example.com/ipfs/CoolStuff");
}
