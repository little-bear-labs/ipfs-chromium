#include <ipfs_client/gateways.h>

#include <google/protobuf/stubs/logging.h>
#include <gtest/gtest.h>

TEST(GatewaysTest, DefaultListMeetsBasicGuidelines) {
  auto dg = ipfs::Gateways::DefaultGateways();
  EXPECT_GT(dg.size(), 9U);
  EXPECT_EQ(dg.at(0).prefix, "http://localhost:8080/");
  for (auto i = 1U; i < dg.size(); ++i) {
    EXPECT_EQ(dg.at(i).prefix.substr(0, 8), std::string{"https://"});
    EXPECT_EQ(dg.at(i).prefix.back(), '/');
    // I considered asserting the number of /s and :s, but that _is_ allowed to
    // change
    EXPECT_LE(dg.at(i).strength, dg.at(i - 1).strength);
  }
}

TEST(GatewaysTest, OverriddenListEndsEntriesInSlash) {
  char env[45] = "IPFS_GATEWAY= a b c d 1 : http://chomp:8080";
  ::putenv(env);
  auto dg = ipfs::Gateways::DefaultGateways();
  EXPECT_EQ(dg.size(), 7U);
  EXPECT_EQ(dg.at(0).prefix, "a/");
  for (auto i = 1U; i < dg.size(); ++i) {
    EXPECT_EQ(dg.at(i).prefix.back(), '/');
    EXPECT_LE(dg.at(i).strength, dg.at(i - 1).strength);
  }
}

TEST(GatewaysTest, AddingNewGatewayShowsUpInList) {
  auto tested = ipfs::Gateways{};
  tested.AddGateways({"newly_added"});
  auto list = tested.GenerateList();
  EXPECT_TRUE(std::any_of(list.begin(), list.end(), [](auto& g) {
    return g.prefix == "http://newly_added/";
  }));
}
