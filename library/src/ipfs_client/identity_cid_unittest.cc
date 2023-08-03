#include <gtest/gtest.h>
#include <ipfs_client/identity_cid.h>
#include <libp2p/multi/content_identifier_codec.hpp>

namespace ut = ipfs::id_cid;
using Codec = libp2p::multi::ContentIdentifierCodec;

TEST(id_cid, from_text) {
  auto result = ut::forText("Ipsum lorem");
  auto s = Codec::toString(result);
  EXPECT_EQ(s.value(), "bafkqac2jobzxk3janrxxezln");
}
