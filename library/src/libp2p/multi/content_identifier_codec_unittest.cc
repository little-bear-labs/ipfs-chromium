#include <gtest/gtest.h>
#include <libp2p/multi/content_identifier_codec.hpp>

#include <sstream>

namespace m = libp2p::multi;
using Codec = m::ContentIdentifierCodec;

TEST(CidCodecTest, StringifyDecodeError) {
  std::ostringstream oss;
  oss << Codec::DecodeError::UNSUPPORTED_MULTIBASE;
  EXPECT_EQ(oss.str(), "Unsupported multibase");
}
