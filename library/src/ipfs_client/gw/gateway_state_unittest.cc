#include "gateway_state.h"

#include <gtest/gtest.h>

#include <ipfs_client/gw/gateway_request.h>

namespace i = ipfs;
namespace ig = i::gw;
using T = ig::GatewayState;
using R = ig::GatewayRequest;

TEST(GatewayStateTest, InitialValues) {
  T t;
  R req;
  EXPECT_TRUE(t.bored());
  for (auto r = 0U; r < 99U; ++r) {
    EXPECT_FALSE(t.over_rate(r));
    EXPECT_EQ(t.score(req, r), r);
  }
}