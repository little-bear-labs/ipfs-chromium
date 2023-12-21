#include <ipfs_client/pb_dag.h>

#include <mock_api.h>

#include <vocab/stringify.h>
#include "log_macros.h"

#include <filesystem>
#include <fstream>

using Cid = ipfs::Cid;

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
  auto cid = Cid{"bafybeigj4sef5qdwdxisatzjdoetjcfdyfqvzuhmstrj5o2ibkbix6vs5i"};
  EXPECT_TRUE(cid.valid());
  ipfs::PbDag block{cid, ipfs::as_bytes(block_bytes)};
  EXPECT_TRUE(block.valid());
  EXPECT_TRUE(block.is_file());
  MockApi api;
  EXPECT_TRUE(block.cid_matches_data(api));
}
TEST(BlockTest, IdentityBlockValidates) {
  std::string block_bytes(
      "\x0a\x08\x08\x02\x12\x02"
      "a\n"
      "\x18\x02",
      10UL);
  EXPECT_EQ(block_bytes.size(), 10UL);
  EXPECT_EQ(block_bytes.at(6), 'a');
  auto cid = Cid{"bafkqac2jobzxk3janrxxezln"};
  ipfs::PbDag block{cid, block_bytes};
  EXPECT_TRUE(block.valid());
  EXPECT_TRUE(block.is_file());
  MockApi api;
  // if this fails, awesome. Change the block_bytes to "Ipsum lorem"
  EXPECT_TRUE(block.cid_matches_data(api));
}
TEST(BlockTest, DirectoryCopiedIsStillDirectory) {
  std::string block_bytes("\x0a\x02\x08\x01");
  EXPECT_EQ(block_bytes.size(), 4U);
  auto cid = Cid{"bafyaabakaieac"};
  EXPECT_TRUE(cid.valid());
  ipfs::PbDag block{cid, block_bytes};
  EXPECT_TRUE(block.valid());
  EXPECT_FALSE(block.is_file());
  MockApi api;
  EXPECT_TRUE(block.cid_matches_data(api));
  EXPECT_EQ(ipfs::Stringify(block.type()), "Directory");
  auto copy = block;
  EXPECT_TRUE(copy.valid());
  EXPECT_FALSE(copy.is_file());
  EXPECT_TRUE(copy.cid_matches_data(api));
  EXPECT_EQ(ipfs::Stringify(copy.type()), "Directory");
}

TEST(BlockTest, AdHoc) {
  using namespace std::filesystem;
  std::for_each(directory_iterator("."), directory_iterator(), [](auto e) {
    if (is_regular_file(e) && e.path().extension() == ".block") {
      LOG(INFO) << e.path();
      auto cid = Cid(e.path().stem().string());
      EXPECT_TRUE(cid.valid());
      std::ifstream f{e.path()};
      std::stringstream buffer;
      buffer << f.rdbuf();
      ipfs::PbDag block{cid, buffer.str()};
      MockApi api;
      EXPECT_TRUE(block.cid_matches_data(api)) << e.path();
      L_VAR(static_cast<long>(block.type()));
      block.List([](auto& name, auto cid) {
        L_VAR(name);
        L_VAR(cid);
        EXPECT_GT(name.size(), 0U);
        EXPECT_GT(cid.size(), 0U);
        return true;
      });
    }
  });
}

TEST(BlockTest, TypeNames) {
  using namespace ipfs;
  using T = PbDag::Type;
  EXPECT_EQ(Stringify(T::Raw), "Raw");
  EXPECT_EQ(Stringify(T::Directory), "Directory");
  EXPECT_EQ(Stringify(T::File), "File");
  EXPECT_EQ(Stringify(T::Metadata), "Metadata");
  EXPECT_EQ(Stringify(T::Symlink), "Symlink");
  EXPECT_EQ(Stringify(T::HAMTShard), "HAMTShard");
  EXPECT_EQ(Stringify(T::FileChunk), "FileChunk");
  EXPECT_EQ(Stringify(T::NonFs), "NonFs");
  EXPECT_EQ(Stringify(T::Invalid), "Invalid");
}
