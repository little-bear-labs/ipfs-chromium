#include <ipfs_client/gateway.h>

#include <gtest/gtest.h>

using G = ipfs::Gateway;

TEST(GatewayTest,PriorityGreater) {
  G a{"a", 1};
  G z{"z", 9};
  EXPECT_TRUE(z < a);
}