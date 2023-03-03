#include <ipfs_client/unixfs_path_resolver.h>

#include <ipfs_client/block_storage.h>
#include <ipfs_client/framework_api.h>
#include <libp2p/multi/multibase_codec/codecs/base58.hpp>
#include "pb_dag.pb.h"
#include "unix_fs.pb.h"

#include <gtest/gtest.h>

namespace {
using entry = std::pair<std::string, std::string>;  // name to CID
ipfs::Block mock_directory(std::vector<entry>);
ipfs::Block mock_file(std::string content);
void setup(std::shared_ptr<ipfs::FrameworkApi>, ipfs::BlockStorage& store);
struct Api final : public ipfs::FrameworkApi {
  std::vector<ipfs::BusyGateway> bgws;
  std::string bytes_received;
  void InitiateGatewayRequest(ipfs::BusyGateway) {
    FAIL() << "InitiateGatewayRequest should not have been called.";
  }
  void RequestByCid(std::string requested, ipfs::Scheduler::Priority) {
    EXPECT_EQ("Nothing additional should be requested", requested);
  }
  std::string MimeTypeFromExtension(std::string extension) const {
    return "text/html";
  }
  void ReceiveBlockBytes(std::string_view b) { bytes_received.append(b); }
  std::string MimeTypeFromContent(std::string_view content,
                                  std::string const& url) const {
    throw 8;
  }
  std::string MimeType(std::string,
                       std::string_view,
                       std::string const&) const {
    return "text/html";
  }
  bool is_done = false;
  void BlocksComplete(std::string) { is_done = true; }
  std::string UnescapeUrlComponent(std::string_view s) const {
    return std::string{s};
  }
  void FourOhFour(std::string_view, std::string_view) {}
};
}  // namespace

TEST(UnixFsPathResolverTest, ResolveDirectoryToIndexHtml) {
  auto api = std::make_shared<Api>();
  ipfs::BlockStorage storage;
  setup(api, storage);
  auto resolver = std::make_shared<ipfs::UnixFsPathResolver>(
      storage, "QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH", "/adir/");
  EXPECT_EQ("QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH",
            resolver->waiting_on());
  EXPECT_EQ(api->is_done, false);
  resolver->Step(api);
  EXPECT_EQ(api->is_done, true);
  EXPECT_EQ(api->bytes_received, "<html><body><p>Hello</p></body></html>\n");
}
TEST(UnixFsPathResolverTest, ResolveDirectoryToGeneratedListing) {
  auto api = std::make_shared<Api>();
  ipfs::BlockStorage storage;
  setup(api, storage);
  auto resolver = std::make_shared<ipfs::UnixFsPathResolver>(
      storage, "QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH", "/");
  EXPECT_EQ("QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH",
            resolver->waiting_on());
  EXPECT_EQ(api->is_done, false);
  resolver->Step(api);
  EXPECT_EQ(api->is_done, true);
  EXPECT_TRUE(api->bytes_received.find(">adir</") < api->bytes_received.size())
      << api->bytes_received;
}

namespace {
void setup(std::shared_ptr<ipfs::FrameworkApi> api, ipfs::BlockStorage& store) {
  store.Store(api, "QmfPDVqow93WH4PjW8PyddPs7D3c6h7njaXGBzpbSgQBZX",
              mock_file("Please ignore\n"));
  store.Store(api,
              "bafybeih5h3u5vqle7laz4kpo3imumdedj2n6y4u5s6zadqgtg77l243zny",
              mock_file("Please ignore\n"));
  store.Store(api, "QmcuGriuDDhMb6hRW71Nt87aDLrqMrh6W3sqxg3H76xEoR",
              mock_file("<html><body><p>Hello</p></body></html>\n"));
  store.Store(api,
              "bafybeigyl4jx7snmutilxzewapa4l3qfqlvorxi7qibjtzurhtti5dm5aa",
              mock_file("<html><body><p>Hello</p></body></html>\n"));
  store.Store(api, "QmSsWZwmg7ArN7KCn1hYpZyQAK5eAryqkBehbRRWbkisFG",
              mock_file("Also ignore\n"));
  store.Store(
      api, "Qmdf4ByEwZtD78Wa2jQmXeQM16xM86P94JmBDjSqsuFXwh",
      mock_directory(
          {{"ignored.txt", "QmfPDVqow93WH4PjW8PyddPs7D3c6h7njaXGBzpbSgQBZX"},
           {"index.html", "QmcuGriuDDhMb6hRW71Nt87aDLrqMrh6W3sqxg3H76xEoR"}}));
  store.Store(
      api, "bafybeihdszhm5xieyovwwqi256pltr5tlvfxzgfxnne7qjm54owrbfyrzq",
      mock_directory(
          {{"ignored.txt", "QmfPDVqow93WH4PjW8PyddPs7D3c6h7njaXGBzpbSgQBZX"},
           {"index.html", "QmcuGriuDDhMb6hRW71Nt87aDLrqMrh6W3sqxg3H76xEoR"}}));
  store.Store(
      api, "QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH",
      mock_directory(
          {{"adir", "Qmdf4ByEwZtD78Wa2jQmXeQM16xM86P94JmBDjSqsuFXwh"}}));
  store.Store(
      api, "bafybeidswjht6punszuomoi6ee2jlcyaz5cislrxa2bbc566tsyycdaxea",
      mock_directory(
          {{"adir", "Qmdf4ByEwZtD78Wa2jQmXeQM16xM86P94JmBDjSqsuFXwh"}}));
}
ipfs::Block mock_directory(std::vector<entry> entries) {
  ipfs::pb_dag::PBNode node;
  for (auto& entry : entries) {
    auto link = node.add_links();
    link->set_name(entry.first);
    auto bytes = libp2p::multi::detail::decodeBase58(entry.second).value();
    link->set_hash(bytes.data(), bytes.size());
  }
  ipfs::unix_fs::Data dir_data;
  dir_data.set_type(ipfs::unix_fs::Data_DataType_Directory);
  node.set_data(dir_data.SerializeAsString());
  return ipfs::Block{libp2p::multi::MulticodecType::Code::DAG_PB,
                     node.SerializeAsString()};
}
ipfs::Block mock_file(std::string content) {
  ipfs::pb_dag::PBNode node;
  ipfs::unix_fs::Data file_data;
  file_data.set_type(ipfs::unix_fs::Data_DataType_File);
  file_data.set_data(content);
  node.set_data(file_data.SerializeAsString());
  return ipfs::Block{libp2p::multi::MulticodecType::Code::DAG_PB,
                     node.SerializeAsString()};
}
}  // namespace
