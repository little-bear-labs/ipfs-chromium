#include <gtest/gtest.h>
#include <libp2p/multi/multibase_codec/codecs/base36.hpp>

#include <libp2p/multi/multibase_codec/codecs/base32.hpp>

TEST(Base36Tests, KnownConversionFrom32) {
  /*
  std::array<short, 40> expected{
      0x01, 0x72, 0x00, 0x24, 0x08, 0x01, 0x12, 0x20, 0x5f, 0x0a,
      0x3b, 0x0d, 0x2c, 0xe9, 0x87, 0x90, 0x97, 0x43, 0xb3, 0xc1,
      0x52, 0xd0, 0x80, 0x82, 0x73, 0xc0, 0x34, 0xbd, 0x2d, 0x63,
      0xfc, 0x85, 0x08, 0xfb, 0x82, 0xe2, 0x1a, 0x4d, 0x53, 0xe2};
  */
  auto result = libp2p::multi::detail::decodeBase32Lower(
      "afzaajaiaejcaxykhmgsz2mhscluhm6bkliibattya2l2lld7scqr64c4ine2u7c");
  EXPECT_TRUE(result.has_value());
  auto expected = result.value();
  result = libp2p::multi::detail::decodeBase36(
      "51qzi5uqu5dijv526o4z2z10ejylnel0bfvrtw53itcmsecffo8yf0zb4g9gi");
  EXPECT_TRUE(result.has_value());
  auto actual = result.value();
  EXPECT_EQ(expected.size(), actual.size());
  for (auto i = 0U; i < actual.size() && i < expected.size(); ++i) {
    ASSERT_EQ(static_cast<unsigned>(expected[i]),
              static_cast<unsigned>(actual[i]))
        << " @ " << i;
  }
}
