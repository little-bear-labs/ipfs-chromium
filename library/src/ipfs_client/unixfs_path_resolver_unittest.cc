#include <ipfs_client/unixfs_path_resolver.h>

#include <ipfs_client/block_storage.h>
#include <ipfs_client/networking_api.h>
#include <libp2p/multi/content_identifier_codec.hpp>
#include <libp2p/multi/multibase_codec/codecs/base58.hpp>
#include "components/ipfs/pb_dag.pb.h"
#include "components/ipfs/unix_fs.pb.h"

#include <gtest/gtest.h>

#include "log_macros.h"

#include <filesystem>
#include <fstream>

namespace {
using entry = std::pair<std::string, std::string>;  // name to CID
ipfs::Block mock_directory(std::string, std::vector<entry>);
ipfs::Block mock_file(std::string, std::string content);
void setup(std::shared_ptr<ipfs::NetworkingApi>, ipfs::BlockStorage& store);
ipfs::GatewayList gwl();
struct Api final : public ipfs::NetworkingApi {
  std::vector<ipfs::BusyGateway> bgws;
  //  void InitiateGatewayRequest(ipfs::BusyGateway) {
  std::shared_ptr<ipfs::GatewayRequest> InitiateGatewayRequest(
      ipfs::BusyGateway) {
    EXPECT_EQ("It was called!",
              "InitiateGatewayRequest should not have been called.");
    return {};
  }
  void RequestByCid(std::string cid,
                    std::shared_ptr<ipfs::DagListener>,
                    ipfs::Priority priority) {
    if (priority) {
      auto ec = std::system(("pwd && ./incblock.sh " + cid).c_str());
      EXPECT_EQ(ec, 0);
      EXPECT_EQ("Nothing additional should be requested", cid);
    }
  }
  void Discover(std::function<void(std::vector<std::string>)>) {}
  std::string MimeTypeFromExtension(std::string extension) const {
    return "text/html";
  }
  std::string MimeTypeFromContent(std::string_view content,
                                  std::string const& url) const {
    throw 8;
  }
  std::size_t head_size = 0UL;
  std::string MimeType(std::string ext,
                       std::string_view cont,
                       std::string const& url) const {
    if (head_size) {
      EXPECT_EQ(cont.size(), head_size);
    }
    LOG(INFO) << "MimeType(ext=" << ext << ",cont.size=" << cont.size()
              << ",url" << url << ')';
    return ext;
  }
  std::string UnescapeUrlComponent(std::string_view s) const {
    return std::string{s};
  }
};
struct Listener : public ipfs::DagListener {
  bool is_done = false;
  std::string bytes_received;
  std::string mime;
  void ReceiveBlockBytes(std::string_view b) { bytes_received.append(b); }
  void BlocksComplete(std::string m) {
    is_done = true;
    mime = m;
  }
  void FourOhFour(std::string_view, std::string_view) {}
};
struct UnixFsPathResolverTest : public ::testing::Test {
  google::protobuf::LogSilencer no_log;
  std::shared_ptr<Api> api = std::make_shared<Api>();
  std::shared_ptr<Listener> listener = std::make_shared<Listener>();
  ipfs::Scheduler sched{gwl};
  ipfs::BlockStorage storage;
};
}  // namespace
TEST_F(UnixFsPathResolverTest, ResolveDirectoryToIndexHtml) {
  setup(api, storage);

  ipfs::UnixFsPathResolver resolver(
      storage, "QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH", "/adir/", api);
  EXPECT_EQ("QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH",
            resolver.current_cid());
  EXPECT_EQ(listener->is_done, false);
  resolver.Step(listener);
  EXPECT_EQ(listener->is_done, true);
  EXPECT_EQ(listener->bytes_received,
            "<html><body><p>Hello</p></body></html>\n");
}
TEST_F(UnixFsPathResolverTest, ReassemblesMultiNodeFile) {
  setup(api, storage);
  auto cid = "bafybeihlj5ay4hmckxzgy43qfnaifjcqoioyw2a6o3pn2ktrlhlqu3df6m";
  ipfs::UnixFsPathResolver resolver(storage, cid, "", api);
  EXPECT_EQ(cid, resolver.current_cid());
  EXPECT_EQ(listener->is_done, false);
  resolver.Step(listener);
  EXPECT_EQ(listener->is_done, true);
  EXPECT_EQ(listener->bytes_received.substr(0, 3), "abc");
  EXPECT_EQ(listener->bytes_received.substr(23, 6), "xyzABC");
  EXPECT_EQ(listener->bytes_received.substr(49, 6), "XYZ012");
  EXPECT_EQ(listener->bytes_received.substr(59, 11), "78900,01,02");
}
TEST_F(UnixFsPathResolverTest, ResolveDirectoryToGeneratedListing) {
  setup(api, storage);
  auto resolver = ipfs::UnixFsPathResolver(
      storage, "QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH", "/", api);
  EXPECT_EQ("QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH",
            resolver.current_cid());
  EXPECT_EQ(listener->is_done, false);
  resolver.Step(listener);
  EXPECT_EQ(listener->is_done, true);
  EXPECT_TRUE(listener->bytes_received.find(">adir</") <
              listener->bytes_received.size())
      << listener->bytes_received;
}
TEST_F(UnixFsPathResolverTest, WikipediaLandingPage) {
  setup(api, storage);
  auto resolver = ipfs::UnixFsPathResolver(
      storage, "bafybeiaysi4s6lnjev27ln5icwm6tueaw2vdykrtjkwiphwekaywqhcjze",
      "/wiki/", api);
  EXPECT_EQ(resolver.current_cid(),
            "bafybeiaysi4s6lnjev27ln5icwm6tueaw2vdykrtjkwiphwekaywqhcjze");
  EXPECT_EQ(listener->is_done, false);
  resolver.Step(listener);
  EXPECT_EQ(listener->is_done, true);
  EXPECT_TRUE(listener->bytes_received.find("6,215,958</span></b> articles") <
              listener->bytes_received.size())
      << listener->bytes_received;
}

TEST_F(UnixFsPathResolverTest, ObservedIncorrectMime) {
  setup(api, storage);
  api->head_size = 262144;
  auto resolver = ipfs::UnixFsPathResolver(
      storage, "QmeVskzFYtigsDBAKuHjtiX1azEUtCtrbXcPi9TK6ES4cX",
      "/assets/index-12705bf9.js", api);
  EXPECT_EQ(resolver.current_cid(),
            "QmeVskzFYtigsDBAKuHjtiX1azEUtCtrbXcPi9TK6ES4cX");
  EXPECT_EQ(listener->is_done, false);
  resolver.Step(listener);
  EXPECT_EQ(listener->is_done, true);
  EXPECT_EQ(listener->mime, "js");
  //  EXPECT_EQ(listener->mime, "text/javascript; charset=utf-8");
}
namespace {
using Codec = libp2p::multi::ContentIdentifierCodec;
void reals(ipfs::BlockStorage& store);
void setup(std::shared_ptr<ipfs::NetworkingApi> api,
           ipfs::BlockStorage& store) {
  store.Store("QmfPDVqow93WH4PjW8PyddPs7D3c6h7njaXGBzpbSgQBZX",
              mock_file("QmfPDVqow93WH4PjW8PyddPs7D3c6h7njaXGBzpbSgQBZX",
                        "Please ignore\n"));
  store.Store(
      mock_file("bafybeih5h3u5vqle7laz4kpo3imumdedj2n6y4u5s6zadqgtg77l243zny",
                "Please ignore\n"));
  store.Store(mock_file("QmcuGriuDDhMb6hRW71Nt87aDLrqMrh6W3sqxg3H76xEoR",
                        "<html><body><p>Hello</p></body></html>\n"));
  store.Store(
      mock_file("bafybeigyl4jx7snmutilxzewapa4l3qfqlvorxi7qibjtzurhtti5dm5aa",
                "<html><body><p>Hello</p></body></html>\n"));
  store.Store(mock_file("QmSsWZwmg7ArN7KCn1hYpZyQAK5eAryqkBehbRRWbkisFG",
                        "Also ignore\n"));
  store.Store(mock_directory(
      "Qmdf4ByEwZtD78Wa2jQmXeQM16xM86P94JmBDjSqsuFXwh",
      {{"ignored.txt", "QmfPDVqow93WH4PjW8PyddPs7D3c6h7njaXGBzpbSgQBZX"},
       {"index.html", "QmcuGriuDDhMb6hRW71Nt87aDLrqMrh6W3sqxg3H76xEoR"}}));
  store.Store(mock_directory(
      "bafybeihdszhm5xieyovwwqi256pltr5tlvfxzgfxnne7qjm54owrbfyrzq",
      {{"ignored.txt", "QmfPDVqow93WH4PjW8PyddPs7D3c6h7njaXGBzpbSgQBZX"},
       {"index.html", "QmcuGriuDDhMb6hRW71Nt87aDLrqMrh6W3sqxg3H76xEoR"}}));
  store.Store(mock_directory(
      "QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH",
      {{"adir", "Qmdf4ByEwZtD78Wa2jQmXeQM16xM86P94JmBDjSqsuFXwh"}}));
  store.Store(
      "bafybeidswjht6punszuomoi6ee2jlcyaz5cislrxa2bbc566tsyycdaxea",
      mock_directory(
          "bafybeidswjht6punszuomoi6ee2jlcyaz5cislrxa2bbc566tsyycdaxea",
          {{"adir", "Qmdf4ByEwZtD78Wa2jQmXeQM16xM86P94JmBDjSqsuFXwh"}}));
  reals(store);
}
ipfs::Block mock_directory(std::string cid_str, std::vector<entry> entries) {
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
  auto cid = Codec::fromString(cid_str).value();
  return ipfs::Block{cid, node.SerializeAsString()};
}
ipfs::Block mock_file(std::string cid_str, std::string content) {
  ipfs::pb_dag::PBNode node;
  ipfs::unix_fs::Data file_data;
  file_data.set_type(ipfs::unix_fs::Data_DataType_File);
  file_data.set_data(content);
  node.set_data(file_data.SerializeAsString());
  auto cid = Codec::fromString(cid_str).value();
  return ipfs::Block{cid, node.SerializeAsString()};
}
ipfs::GatewayList gwl() {
  return {};
}
void reals(ipfs::BlockStorage& store) {
  using P = std::filesystem::path;
  using E = std::filesystem::directory_entry;
  using I = std::filesystem::directory_iterator;
  auto blocs_dir =
      P{__FILE__}.parent_path().parent_path().parent_path().parent_path() /
      "test_data" / "blocks";
  std::for_each(I{blocs_dir}, I{}, [&store](E const& e) {
    if (e.is_regular_file()) {
      auto cid_str = e.path().filename();
      auto cid = Codec::fromString(cid_str).value();
      std::ifstream f{e.path()};
      store.Store(ipfs::Block{cid, f});
    }
  });
}
}  // namespace
