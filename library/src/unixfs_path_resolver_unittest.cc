#include <ipfs_client/unixfs_path_resolver.h>

#include <ipfs_client/block_storage.h>
#include <ipfs_client/gen/pb_dag.pb.h>
#include <ipfs_client/gen/unix_fs.pb.h>
#include <libp2p/multi/multibase_codec/codecs/base58.hpp>

#include <gtest/gtest.h>

namespace {
using entry = std::pair<std::string, std::string>;  // name to CID
ipfs::Block mock_directory(std::vector<entry>);
ipfs::Block mock_file(std::string content);
void setup(ipfs::BlockStorage& store);
}  // namespace

TEST(UnixFsPathResolverTest, ResolveDirectoryToIndexHtml) {
  ipfs::BlockStorage storage;
  setup(storage);
  auto fail = [](auto requested) {
    EXPECT_EQ("Nothing additional should be requested", requested);
  };
  bool is_done = false;
  auto done = [&is_done](auto t) {
    EXPECT_EQ(t, "text/html");
    is_done = true;
  };
  auto index_html_recieved = [](auto file_contents) {
    EXPECT_EQ(file_contents, "<html><body><p>Hello</p></body></html>\n");
  };
  auto resolver = std::make_shared<ipfs::UnixFsPathResolver>(
      storage, "QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH", "/adir/", fail,
      fail, index_html_recieved, done);
  EXPECT_EQ("QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH",
            resolver->waiting_on());
  EXPECT_EQ(is_done, false);
  resolver->Step(resolver);
  EXPECT_EQ("QmcuGriuDDhMb6hRW71Nt87aDLrqMrh6W3sqxg3H76xEoR",
            resolver->waiting_on());
  EXPECT_EQ(is_done, true);
}
TEST(UnixFsPathResolverTest, ResolveDirectoryToGeneratedListing) {
  ipfs::BlockStorage storage;
  setup(storage);
  auto fail = [](auto requested) {
    EXPECT_EQ("Nothing additional should be requested", requested);
  };
  bool is_done = false;
  auto done = [&is_done](auto t) {
    EXPECT_EQ(t, "text/html");
    is_done = true;
  };
  std::string content;
  auto index_html_recieved = [&content](auto file_contents) {
    content.append(file_contents);
  };
  auto resolver = std::make_shared<ipfs::UnixFsPathResolver>(
      storage, "QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH", "/", fail,
      fail, index_html_recieved, done);
  EXPECT_EQ("QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH",
            resolver->waiting_on());
  EXPECT_EQ(is_done, false);
  resolver->Step(resolver);
  EXPECT_EQ(is_done, true);
  EXPECT_TRUE(content.find(">adir</") < content.size()) << content;
}

namespace {
void setup(ipfs::BlockStorage& store) {
  store.Store("QmfPDVqow93WH4PjW8PyddPs7D3c6h7njaXGBzpbSgQBZX",
              mock_file("Please ignore\n"));
  store.Store("QmcuGriuDDhMb6hRW71Nt87aDLrqMrh6W3sqxg3H76xEoR",
              mock_file("<html><body><p>Hello</p></body></html>\n"));
  store.Store("QmSsWZwmg7ArN7KCn1hYpZyQAK5eAryqkBehbRRWbkisFG",
              mock_file("Also ignore\n"));
  store.Store(
      "Qmdf4ByEwZtD78Wa2jQmXeQM16xM86P94JmBDjSqsuFXwh",
      mock_directory(
          {{"ignored.txt", "QmfPDVqow93WH4PjW8PyddPs7D3c6h7njaXGBzpbSgQBZX"},
           {"index.html", "QmcuGriuDDhMb6hRW71Nt87aDLrqMrh6W3sqxg3H76xEoR"}}));
  store.Store(
      "QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH",
      mock_directory(
          {{"adir", "Qmdf4ByEwZtD78Wa2jQmXeQM16xM86P94JmBDjSqsuFXwh"}}));
}
ipfs::Block mock_directory(std::vector<entry> entries) {
  ipfs::pb_dag::PBNode node;
  for (auto& entry : entries) {
    auto link = node.add_links();
    link->set_name(entry.first);
    link->set_hash(libp2p::multi::detail::decodeBase58(entry.second).value());
  }
  ipfs::unix_fs::Data dir_data;
  dir_data.set_type(ipfs::unix_fs::Data_DataType_Directory);
  node.set_data(dir_data.SerializeAsString());
  return ipfs::Block{node.SerializeAsString()};
}
ipfs::Block mock_file(std::string content) {
  ipfs::pb_dag::PBNode node;
  ipfs::unix_fs::Data file_data;
  file_data.set_type(ipfs::unix_fs::Data_DataType_File);
  file_data.set_data(content);
  node.set_data(file_data.SerializeAsString());
  return ipfs::Block{node.SerializeAsString()};
}
}  // namespace
