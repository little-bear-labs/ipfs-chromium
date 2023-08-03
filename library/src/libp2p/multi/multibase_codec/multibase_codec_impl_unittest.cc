#include <gtest/gtest.h>
#include <libp2p/multi/multibase_codec/multibase_codec_impl.hpp>

namespace m = libp2p::multi;
using Codec = m::MultibaseCodec;
using Impl = m::MultibaseCodecImpl;

TEST(MultibaseCodecImplTest, DecodeUnknownUnsupportedMultibase) {
  Impl i;
  auto result =
      i.decode("?Question mark is not even in the table of multibase prefixes");
  EXPECT_TRUE(result.has_error());
  EXPECT_EQ(Codec::Error::UNSUPPORTED_BASE, result.error());
}

TEST(MultibaseCodecImplTest, DecodeKnownUnsupportedMultibase) {
  Impl i;
  auto result = i.decode(
      "mBase64 *IS* found in encodingByChar, but not mapped in codecs array.");
  EXPECT_TRUE(result.has_error());
  EXPECT_EQ(Codec::Error::UNSUPPORTED_BASE, result.error());
}
