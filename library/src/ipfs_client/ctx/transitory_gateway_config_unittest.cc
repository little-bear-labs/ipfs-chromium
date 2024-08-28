#include <ipfs_client/ctx/transitory_gateway_config.h>

#include <gtest/gtest.h>

using T = ipfs::ctx::TransitoryGatewayConfig;
using RT = ipfs::gw::GatewayRequestType;

TEST(TransitoryGatewayConfigTest, EmptyStringEnvLoadsStatic) {
  char env[15] = "IPFS_GATEWAY=";
  putenv(env);
  T c;
  auto first = c.GetGateway(0);
  EXPECT_TRUE(first.has_value());
  auto second = c.GetGateway(1);
  EXPECT_TRUE(second.has_value());
  EXPECT_NE(first->prefix, second->prefix);
  EXPECT_GE(first->rate, second->rate);
  EXPECT_GT(second->rate, 0);
}
TEST(TransitoryGatewayConfigTest, SetGatewayRateObservable) {
  T c;
  auto b4 = c.GetGateway(0);
  ASSERT_TRUE(b4.has_value());
  auto pre = b4.value().prefix;
  auto val = b4.value().rate;
  c.SetGatewayRate(pre, val + 9);
  auto result = c.GetGatewayRate(pre);
  EXPECT_EQ(result, val + 9);
}
TEST(TransitoryGatewayConfigTest, RoutingApiDiscoveryDefaultRateIs60) {
  T t;
  EXPECT_EQ(t.RoutingApiDiscoveryDefaultRate(),60);
}
TEST(TransitoryGatewayConfigTest, RoutingApiDiscoveryOfUnencryptedGatewaysIsFalse) {
  T t;
  EXPECT_FALSE(t.RoutingApiDiscoveryOfUnencryptedGateways());
}
TEST(TransitoryGatewayConfigTest, TypeAffinityIs9) {
  T t;
  EXPECT_EQ(9, t.GetTypeAffinity("whatevs", RT::Identity));
}
