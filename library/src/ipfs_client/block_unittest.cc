#include <ipfs_client/block.h>

#include <gtest/gtest.h>

#include <libp2p/multi/content_identifier_codec.hpp>

#include <filesystem>
#include <fstream>

using Cid = libp2p::multi::ContentIdentifier;
using Codec = libp2p::multi::ContentIdentifierCodec;

TEST(BlockTest, RealBlockValidates) {
  // The actual content in this block is the "a\n" in the middle
  // The rest is metadata about it being a file with 2 bytes etc..
  std::string block_bytes(
      "\x0a\x08\x08\x02\x12\x02"
      "a\n"
      "\x18\x02",
      10UL);
  EXPECT_EQ(block_bytes.size(), 10UL);
  EXPECT_EQ(block_bytes.at(6), 'a');
  auto cid = Codec::fromString(
                 "bafybeigj4sef5qdwdxisatzjdoetjcfdyfqvzuhmstrj5o2ibkbix6vs5i")
                 .value();
  ipfs::Block block{cid, block_bytes};
  EXPECT_TRUE(block.valid());
  EXPECT_TRUE(block.is_file());
  EXPECT_TRUE(block.cid_matches_data());
}

TEST(BlockTest, AdHoc) {
  using namespace std::filesystem;
  std::for_each(directory_iterator("."), directory_iterator(), [](auto e) {
    if (is_regular_file(e) && e.path().extension() == ".block") {
      auto cid = Codec ::fromString(e.path().stem()).value();
      std::ifstream f{e.path()};
      std::stringstream buffer;
      buffer << f.rdbuf();
      ipfs::Block block{cid, buffer.str()};
      EXPECT_TRUE(block.cid_matches_data()) << e.path();
    }
  });
}