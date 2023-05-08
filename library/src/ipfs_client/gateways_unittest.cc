#include <ipfs_client/gateways.h>

#include <google/protobuf/stubs/logging.h>
#include <gtest/gtest.h>

TEST(GatewaysTest, DefaultListMeetsBasicGuidelines) {
  auto dg = ipfs::Gateways::DefaultGateways();
  EXPECT_GT(dg.size(), 9U);
  EXPECT_EQ(dg.at(0).first, "http://localhost:8080/");
  for (auto i = 1U; i < dg.size(); ++i) {
    EXPECT_EQ(dg.at(i).first.substr(0, 8), std::string{"https://"});
    EXPECT_EQ(dg.at(i).first.back(), '/');
    // I considered asserting the number of /s and :s, but that _is_ allowed to
    // change
    EXPECT_LE(dg.at(i).second, dg.at(i - 1).second);
  }
}
