#include <gtest/gtest.h>
#include <ipfs_client/identity_cid.h>

#include <ipfs_client/cid.h>

namespace ut = ipfs::id_cid;

TEST(id_cid, from_text) {
  auto result = ut::forText("Ipsum lorem");
  EXPECT_EQ(result.to_string(), "bafkqac2jobzxk3janrxxezln");
}
