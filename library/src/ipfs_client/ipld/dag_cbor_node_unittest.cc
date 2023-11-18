#include "dag_cbor_node.h"

#include <ipfs_client/json_cbor_adapter.h>

#include <gtest/gtest.h>

#include "chunk.h"

using Tested = ipfs::ipld::DagCborNode;
using namespace std::literals;
namespace i = ipfs;
namespace ii = i::ipld;

namespace {
struct DagCborTest : public ::testing::Test {
  std::shared_ptr<Tested> tested_;
  void parse(std::vector<std::uint8_t> s) {
    auto data = nlohmann::json::from_cbor(s, false, true, nlohmann::detail::cbor_tag_handler_t::store);
    auto obj = std::make_unique<ipfs::JsonCborAdapter>(data);
    tested_ = std::make_shared<Tested>(std::move(obj));
  }
  ipfs::ipld::DagNode& tested() {
    return *tested_;
  }
};
}

TEST_F(DagCborTest,ObjectWithLink) {
  auto metadata_json = R"JSON({"name":"headshot","description":"its me!","image":"ipfs://bafybeibd3nuryjpflpvftsx5ps3d4rr4qjp63xtedtigtjfu7cgtj7o24e/headshot.png"})JSON";
  parse({
      0xa5, 0x64, 0x6e, 0x61, 0x6d, 0x65, 0x68, 0x68, 0x65, 0x61, 0x64, 0x73,
      0x68, 0x6f, 0x74, 0x64, 0x74, 0x79, 0x70, 0x65, 0x63, 0x6e, 0x66, 0x74,
      0x65, 0x69, 0x6d, 0x61, 0x67, 0x65, 0xd8, 0x2a, 0x58, 0x25, 0x00, 0x01,
      0x70, 0x12, 0x20, 0x23, 0xdb, 0x69, 0x1c, 0x25, 0xe5, 0x5b, 0xea, 0x59,
      0xca, 0xfd, 0x7c, 0xb6, 0x3e, 0x46, 0x3c, 0x82, 0x5f, 0xed, 0xde, 0x64,
      0x1c, 0xd0, 0x69, 0xa4, 0xb4, 0xf8, 0x8d, 0x34, 0xfd, 0xda, 0xe1, 0x6b,
      0x64, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x67,
      0x69, 0x74, 0x73, 0x20, 0x6d, 0x65, 0x21, 0x6d, 0x6d, 0x65, 0x74, 0x61,
      0x64, 0x61, 0x74, 0x61, 0x2e, 0x6a, 0x73, 0x6f, 0x6e, 0xd8, 0x2a, 0x58,
      0x25, 0x00, 0x01, 0x55, 0x12, 0x20, 0x4a, 0xac, 0x17, 0x9a, 0xdc, 0x23,
      0xfd, 0xed, 0x79, 0x23, 0xcd, 0x9d, 0x06, 0xb2, 0x05, 0x76, 0x37, 0xd0,
      0x3f, 0xe2, 0xda, 0x9d, 0x2f, 0xbc, 0xd5, 0xa7, 0xeb, 0x59, 0x47, 0x3f,
      0x6b, 0x48
  });
  EXPECT_TRUE(tested_);
  auto blu = [&metadata_json](std::string const& c)-> ipfs::ipld::NodePtr {
      if (c == "bafkreickvqlzvxbd7xwxsi6ntudleblwg7id7yw2tux3zvnh5nmuop3lja") {
        return std::make_shared<ii::Chunk>(metadata_json);
      }
      throw std::runtime_error( "Should already have all necessary blocks, requested " + c );
      return ipfs::ipld::NodePtr{};
    };
  std::string to_here{"/ipfs/bafyreifrkkhbwkf3fibwhbzcmpqnmh3b4t5t4ig7qdtymkdoatcycwe6n4"};
  auto result = tested().resolve(""sv, blu, to_here);
  auto& response = std::get<ipfs::Response>(result);
  EXPECT_EQ(response.mime_,"text/html");
  EXPECT_EQ(response.body_, R"HTML(<html><title>DAG-CBOR Preview</title><body>
<table class='cbor_map' border=1>
  <tr><td>description</td><td>
<p class='cbor_string'><em>&quot;</em>its me!<em>&quot;</em></p>
  </td></tr>
  <tr><td>image</td><td>
<a class='cbor_link' href='ipfs://bafybeibd3nuryjpflpvftsx5ps3d4rr4qjp63xtedtigtjfu7cgtj7o24e'>bafybeibd3nuryjpflpvftsx5ps3d4rr4qjp63xtedtigtjfu7cgtj7o24e</a>
  </td></tr>
  <tr><td>metadata.json</td><td>
<a class='cbor_link' href='ipfs://bafkreickvqlzvxbd7xwxsi6ntudleblwg7id7yw2tux3zvnh5nmuop3lja'>bafkreickvqlzvxbd7xwxsi6ntudleblwg7id7yw2tux3zvnh5nmuop3lja</a>
  </td></tr>
  <tr><td>name</td><td>
<p class='cbor_string'><em>&quot;</em>headshot<em>&quot;</em></p>
  </td></tr>
  <tr><td>type</td><td>
<p class='cbor_string'><em>&quot;</em>nft<em>&quot;</em></p>
  </td></tr>
</table>
</body></html>)HTML"
            );
  result = tested().resolve("metadata.json"sv, blu, to_here);
  auto resp = std::get<ipfs::Response>(result);
  EXPECT_EQ(resp.status_,200);
  EXPECT_EQ(resp.body_,metadata_json);
}
