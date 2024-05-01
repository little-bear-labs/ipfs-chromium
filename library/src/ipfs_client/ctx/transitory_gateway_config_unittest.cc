#include <ipfs_client/ctx/transitory_gateway_config.h>

#include <gtest/gtest.h>

#include <stdlib.h>

TEST(TransitoryGatewayConfigTest, EmptyStringEnvLoadsStatic) {
  char env[15] = "IPFS_GATEWAY=";
  putenv(env);
  ipfs::ctx::TransitoryGatewayConfig c;
  auto first = c.GetGateway(0);
  EXPECT_TRUE(first.has_value());
  auto second = c.GetGateway(1);
  EXPECT_TRUE(second.has_value());
  EXPECT_NE(first->prefix, second->prefix);
  EXPECT_GE(first->rate, second->rate);
  EXPECT_GT(second->rate, 0);
}
