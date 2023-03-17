#include <ipfs_client/unixfs_path_resolver.h>

#include <ipfs_client/block_storage.h>
#include <ipfs_client/framework_api.h>
#include <libp2p/multi/content_identifier_codec.hpp>
#include <libp2p/multi/multibase_codec/codecs/base58.hpp>
#include "pb_dag.pb.h"
#include "unix_fs.pb.h"

#include <gtest/gtest.h>
/*

namespace {
using entry = std::pair<std::string, std::string>;  // name to CID
ipfs::Block mock_directory(std::string, std::vector<entry>);
ipfs::Block mock_file(std::string, std::string content);
void setup(std::shared_ptr<ipfs::FrameworkApi>, ipfs::BlockStorage& store);
struct Api final : public ipfs::FrameworkApi {
  std::vector<ipfs::BusyGateway> bgws;
  void InitiateGatewayRequest(ipfs::BusyGateway) {
    FAIL() << "InitiateGatewayRequest should not have been called.";
  }
  void RequestByCid(std::string requested, ipfs::Scheduler::Priority) {
    EXPECT_EQ("Nothing additional should be requested", requested);
  }
  std::string MimeTypeFromExtension(std::string extension) const {
    return "text/html";
  }
  std::string MimeTypeFromContent(std::string_view content,
                                  std::string const& url) const {
    throw 8;
  }
  std::string MimeType(std::string,
                       std::string_view,
                       std::string const&) const {
    return "text/html";
  }
  std::string UnescapeUrlComponent(std::string_view s) const {
    return std::string{s};
  }
};
struct Listener : public ipfs::DagListener {
  bool is_done = false;
  std::string bytes_received;
  void ReceiveBlockBytes(std::string_view b) { bytes_received.append(b); }
  void BlocksComplete(std::string) { is_done = true; }
  void FourOhFour(std::string_view, std::string_view) {}
};
struct UnixFsPathResolverTest : public ::testing::Test {
  std::shared_ptr<Api> api = std::make_shared<Api>();
  std::shared_ptr<Listener> listener = std::make_shared<Listener>();
};
}  // namespace
   TEST_F(UnixFsPathResolverTest, ResolveDirectoryToIndexHtml) {
     ipfs::BlockStorage storage;
     setup(api, storage);
     ipfs::UnixFsPathResolver resolver(
         storage, {}, "QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH",
"/adir/", listener, api);
     EXPECT_EQ("QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH",
               resolver.waiting_on());
     EXPECT_EQ(listener->is_done, false);
     resolver.Step();
     EXPECT_EQ(listener->is_done, true);
     EXPECT_EQ(listener->bytes_received,
               "<html><body><p>Hello</p></body></html>\n");
   }
   TEST_F(UnixFsPathResolverTest, ResolveDirectoryToGeneratedListing) {
     auto api = std::make_shared<Api>();
     ipfs::BlockStorage storage;
     setup(api, storage);
     auto resolver = ipfs::UnixFsPathResolver(
         storage, {}, "QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH", "/",
         listener, api);
     EXPECT_EQ("QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH",
               resolver.waiting_on());
     EXPECT_EQ(listener->is_done, false);
     resolver.Step();
     EXPECT_EQ(listener->is_done, true);
     EXPECT_TRUE(listener->bytes_received.find(">adir</") <
                 listener->bytes_received.size())
         << listener->bytes_received;
   }

   namespace {
   void setup(std::shared_ptr<ipfs::FrameworkApi> api, ipfs::BlockStorage&
store) { store.Store(api, "QmfPDVqow93WH4PjW8PyddPs7D3c6h7njaXGBzpbSgQBZX",
                 mock_file("QmfPDVqow93WH4PjW8PyddPs7D3c6h7njaXGBzpbSgQBZX",
                           "Please ignore\n"));
     store.Store(
         api,

         mock_file("bafybeih5h3u5vqle7laz4kpo3imumdedj2n6y4u5s6zadqgtg77l243zny",
                   "Please ignore\n"));
     store.Store(api,
mock_file("QmcuGriuDDhMb6hRW71Nt87aDLrqMrh6W3sqxg3H76xEoR",
                                "<html><body><p>Hello</p></body></html>\n"));
     store.Store(
         api,

         mock_file("bafybeigyl4jx7snmutilxzewapa4l3qfqlvorxi7qibjtzurhtti5dm5aa",
                   "<html><body><p>Hello</p></body></html>\n"));
     store.Store(api,
mock_file("QmSsWZwmg7ArN7KCn1hYpZyQAK5eAryqkBehbRRWbkisFG", "Also ignore\n"));
     store.Store(
         api,
         mock_directory(
             "Qmdf4ByEwZtD78Wa2jQmXeQM16xM86P94JmBDjSqsuFXwh",
             {{"ignored.txt", "QmfPDVqow93WH4PjW8PyddPs7D3c6h7njaXGBzpbSgQBZX"},
              {"index.html",
"QmcuGriuDDhMb6hRW71Nt87aDLrqMrh6W3sqxg3H76xEoR"}})); store.Store( api,
         mock_directory(
             "bafybeihdszhm5xieyovwwqi256pltr5tlvfxzgfxnne7qjm54owrbfyrzq",
             {{"ignored.txt", "QmfPDVqow93WH4PjW8PyddPs7D3c6h7njaXGBzpbSgQBZX"},
              {"index.html",
"QmcuGriuDDhMb6hRW71Nt87aDLrqMrh6W3sqxg3H76xEoR"}})); store.Store( api,
mock_directory( "QmW4NtHG2Q85KaCzPQJrziWATQ2T2SQUQEnVzsN9YocNTH",
                  {{"adir",
"Qmdf4ByEwZtD78Wa2jQmXeQM16xM86P94JmBDjSqsuFXwh"}})); store.Store( api,
"bafybeidswjht6punszuomoi6ee2jlcyaz5cislrxa2bbc566tsyycdaxea", mock_directory(
             "bafybeidswjht6punszuomoi6ee2jlcyaz5cislrxa2bbc566tsyycdaxea",
             {{"adir", "Qmdf4ByEwZtD78Wa2jQmXeQM16xM86P94JmBDjSqsuFXwh"}}));
   }
   using Codec = libp2p::multi::ContentIdentifierCodec;
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
   }  // namespace
   */