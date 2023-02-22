#include "ipfs_client/scheduler.h"

#include <ipfs_client/framework_api.h>

#include <gtest/gtest.h>

namespace {
struct Api final : public ipfs::FrameworkApi {
  std::vector<ipfs::BusyGateway> bgws;
  void InitiateGatewayRequest(ipfs::BusyGateway bgw) {
    bgws.push_back(std::move(bgw));
  }
  void RequestByCid(std::string, ipfs::Scheduler::Priority) {}
  std::string MimeTypeFromExtension(std::string extension) const { throw 9; }
  std::string MimeTypeFromContent(std::string_view content,
                                  std::string const& url) const {
    throw 8;
  }
  void ReceiveBlockBytes(std::string_view b) {
    EXPECT_EQ("not supposed to call", b);
  }
  void BlocksComplete(std::string) {}
};
}  // namespace

TEST(IpfsSchedulerTest, AssignBusyGateway) {
  ipfs::GatewayList gws{ipfs::Gateway{"https://example.com/", 99}};

  auto api = std::make_shared<Api>();
  ipfs::Scheduler sched(std::move(gws));
  sched.Enqueue(api, "ipfs/CoolStuff", ipfs::Scheduler::Priority::Required);
  EXPECT_EQ(api->bgws.size(), 1UL);
  EXPECT_TRUE(api->bgws.at(0));
  EXPECT_EQ(api->bgws.at(0)->url(), "https://example.com/ipfs/CoolStuff");
}
