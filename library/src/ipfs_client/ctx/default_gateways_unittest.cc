#include <ipfs_client/ctx/default_gateways.h>

#include <gtest/gtest.h>

#include <ipfs_client/ctx/transitory_gateway_config.h>

#include <cstdlib>

namespace ic = ipfs::ctx;

TEST(DefaultGatewaysTest,EnvironmentOverride) {
  auto failed_to_set_env = ::setenv("IPFS_GATEWAY", "a b", 1);
  EXPECT_EQ(failed_to_set_env,0);
  ic::TransitoryGatewayConfig cfg;
  auto overridden = ic::LoadGatewaysFromEnvironmentVariable(cfg);
  ::unsetenv("IPFS_GATEWAY");
  EXPECT_TRUE(overridden);
  auto a = cfg.GetGateway(0);
  ASSERT_TRUE(a.has_value());
  EXPECT_EQ(a.value().prefix,"a/");
  auto b = cfg.GetGateway(1);
  ASSERT_TRUE(b.has_value());
  EXPECT_EQ(b.value().prefix,"b/");
}
