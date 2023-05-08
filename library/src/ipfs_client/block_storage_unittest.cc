#include <ipfs_client/block_storage.h>

#include <gtest/gtest.h>

#include <ipfs_client/dag_listener.h>

namespace {
struct BlockStorageTest : public ::testing::Test, public ipfs::DagListener {
  ipfs::BlockStorage under_test;
  std::vector<std::string> cids_searched_for_in_cache;
  BlockStorageTest() {
    under_test.cache_search_initiator([this](auto s) { SearchCache(s); });
  }
  void SearchCache(std::string cid) {
    cids_searched_for_in_cache.push_back(cid);
  }
  std::string bytes_heard;
  std::string mime_completed;
  std::string unfound;
  void ReceiveBlockBytes(std::string_view bytes) { bytes_heard.append(bytes); }
  void BlocksComplete(std::string mime_type) { mime_completed = mime_type; }
  void FourOhFour(std::string_view cid, std::string_view path) {
    unfound.append(cid).append("/").append(path);
  }
};
}  // namespace

TEST_F(BlockStorageTest, ChecksCache) {
  under_test.Get("unfound");
  EXPECT_EQ(cids_searched_for_in_cache.size(), 1UL);
  EXPECT_EQ(cids_searched_for_in_cache.at(0), "unfound");
  EXPECT_EQ(bytes_heard.size(), 0U);
  EXPECT_EQ(mime_completed.size(), 0U);
  EXPECT_EQ(unfound.size(), 0U);
}
TEST_F(BlockStorageTest, IoPending_Found) {
  under_test.Get("unfound");
  EXPECT_EQ(cids_searched_for_in_cache.size(), 1UL);
  EXPECT_EQ(cids_searched_for_in_cache.at(0), "unfound");
  EXPECT_EQ(bytes_heard.size(), 0U);
  EXPECT_EQ(mime_completed.size(), 0U);
  EXPECT_EQ(unfound.size(), 0U);
}
