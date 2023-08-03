#include <gtest/gtest.h>
#include <libp2p/multi/multibase_codec/codecs/base16.h>

namespace b16 = ipfs::base16;

TEST(Base16Test, EncodeLower) {
  auto xyz = reinterpret_cast<ipfs::Byte const*>("xyz");
  auto actual = b16::encodeLower({xyz, 3});
  EXPECT_EQ(actual, "78797a");
}
TEST(Base16Test, DecodeUpper) {
  std::string s = "78797A";
  auto expected = reinterpret_cast<ipfs::Byte const*>("xyz");
  auto result = b16::decode(s);
  EXPECT_TRUE(result.has_value());
  auto actual = result.value();
  EXPECT_EQ(3, actual.size());
  for (auto i : {0, 1, 2}) {
    EXPECT_EQ((short)(actual.at(i)), (short)(expected[i]));
  }
}
