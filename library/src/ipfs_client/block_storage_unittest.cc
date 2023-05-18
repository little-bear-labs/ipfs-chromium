#include <ipfs_client/block_storage.h>

#include <gtest/gtest.h>

#include <ipfs_client/dag_listener.h>

#include <libp2p/multi/content_identifier_codec.hpp>

using Codec = libp2p::multi::ContentIdentifierCodec;
using namespace std::literals;

namespace {
struct TestListener : public ipfs::DagListener {
  void ReceiveBlockBytes(std::string_view) override {}
  void BlocksComplete(std::string mime_type) override {}
  void FourOhFour(std::string_view cid, std::string_view path) override {}
};

struct BlockStorageTest : public testing::Test {
  ipfs::BlockStorage under_test;
  BlockStorageTest() {}
  ipfs::Cid abc_cid =
      Codec::fromString(
          "bafkreif2pall7dybz7vecqka3zo24irdwabwdi4wc55jznaq75q7eaavvu")
          .value();
};
}  // namespace

TEST_F(BlockStorageTest, StoringADuplicate) {
  auto ret = under_test.Store("", "abc", ipfs::Block{abc_cid, "abc"s});
  EXPECT_EQ(ret, true);
  ret = under_test.Store("", "abc", ipfs::Block{abc_cid, "abc"s});
  EXPECT_EQ(ret, false);
}
