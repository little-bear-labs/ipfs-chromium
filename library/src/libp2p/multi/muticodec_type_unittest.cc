#include <libp2p/multi/multicodec_type.hpp>

#include <gtest/gtest.h>

using T = libp2p::multi::MulticodecType;
using C = T::Code;

TEST(MultiCodecType, Codes) {
  EXPECT_EQ(static_cast<int>(C::IDENTITY), 0x00);
  EXPECT_EQ(static_cast<int>(C::SHA1), 0x11);
  EXPECT_EQ(static_cast<int>(C::SHA2_256), 0x12);
  EXPECT_EQ(static_cast<int>(C::SHA2_512), 0x13);
  EXPECT_EQ(static_cast<int>(C::SHA3_512), 0x14);
  EXPECT_EQ(static_cast<int>(C::SHA3_384), 0x15);
  EXPECT_EQ(static_cast<int>(C::SHA3_256), 0x16);
  EXPECT_EQ(static_cast<int>(C::SHA3_224), 0x17);
  EXPECT_EQ(static_cast<int>(C::RAW), 0x55);
  EXPECT_EQ(static_cast<int>(C::DAG_PB), 0x70);
  EXPECT_EQ(static_cast<int>(C::DAG_CBOR), 0x71);
  EXPECT_EQ(static_cast<int>(C::LIBP2P_KEY), 0x72);
  EXPECT_EQ(static_cast<int>(C::DAG_JSON), 0x0129);
  EXPECT_EQ(static_cast<int>(C::FILECOIN_COMMITMENT_UNSEALED), 0xf101);
  EXPECT_EQ(static_cast<int>(C::FILECOIN_COMMITMENT_SEALED), 0xf102);
}
TEST(MultiCodecType, Names) {
  EXPECT_EQ(T::getName(C::IDENTITY), "identity");
  EXPECT_EQ(T::getName(C::SHA1), "sha1");
  EXPECT_EQ(T::getName(C::SHA2_256), "sha2-256");
  EXPECT_EQ(T::getName(C::SHA2_512), "sha2-512");
  EXPECT_EQ(T::getName(C::SHA3_512), "sha3-512");
  EXPECT_EQ(T::getName(C::SHA3_384), "sha3-384");
  EXPECT_EQ(T::getName(C::SHA3_256), "sha3-256");
  EXPECT_EQ(T::getName(C::SHA3_224), "sha3-224");
  EXPECT_EQ(T::getName(C::RAW), "raw");
  EXPECT_EQ(T::getName(C::DAG_PB), "dag-pb");
  EXPECT_EQ(T::getName(C::DAG_CBOR), "dag-cbor");
  EXPECT_EQ(T::getName(C::LIBP2P_KEY), "libp2p-key");
  EXPECT_EQ(T::getName(C::DAG_JSON), "dag-json");
  EXPECT_EQ(T::getName(C::FILECOIN_COMMITMENT_UNSEALED),
            "fil-commitment-unsealed");
  EXPECT_EQ(T::getName(C::FILECOIN_COMMITMENT_SEALED), "fil-commitment-sealed");
}