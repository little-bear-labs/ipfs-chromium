#include "symlink.h"

#include "small_directory.h"

#include <mock_api.h>

#include <ipfs_client/ipld/dag_node.h>
#include <ipfs_client/pb_dag.h>

using namespace std::literals;
namespace i = ipfs;
namespace ii = i::ipld;

TEST(SymlinkTest, fromBlock) {
  i::Cid cid("bafybeia4wauf6z6lnnnszia6upqr5jsq7nack5nnrubf333lfg2vlabtd4");
  EXPECT_TRUE(cid.valid());
  // The target is the last byte, \x61 aka 'a'
  i::PbDag b{cid, "\x0a\x05\x08\x04\x12\x01\x61"};
  EXPECT_TRUE(b.type() == i::PbDag::Type::Symlink);
  auto node = ii::DagNode::fromBlock(b);
  EXPECT_TRUE(node);
  auto blu = [](std::string const&) { return ii::NodePtr{}; };
  auto result = node->resolve(i::SlashDelimited{"d/e"}, blu);
  auto actual = std::get<ii::PathChange>(result);
  auto expect = "/a/d/e";
  EXPECT_EQ(actual.new_path, expect);
}
TEST(SymlinkTest, rooted) {
  auto target = "/b";
  auto sub = std::make_shared<ii::Symlink>(target);
  // This dir actually contains an inlined symlink which points to /b/c, but
  // it's the Symlink ctor arg that counts
  std::string dirbytes =
      "\x12\x15\x0a\x0e\x01\x70\x00\x0a\x0a\x08\x08\x04\x12\x04\x2f\x62\x2f\x63"
      "\x12\x01\x61\x18\x0a\x0a\x02\x08\x01";
  auto api = std::make_shared<MockApi>();
  auto dir = ii::DagNode::fromBytes(
      api, i::Cid("bafyaagiscmfayalqaaeaubqiaqjael3cciawcgaibibaqai"),
      dirbytes);
  ii::DagNode& t = *sub;
  auto blu = [sub](std::string const& block_key) -> ii::NodePtr {
    EXPECT_EQ("bafyaacakayeaieqcf5ra", block_key);
    return sub;
  };
  auto res = t.resolve(i::SlashDelimited{}, blu);
  EXPECT_TRUE(std::holds_alternative<ii::PathChange>(res));
  EXPECT_EQ(std::get<ii::PathChange>(res).new_path, target);
}
