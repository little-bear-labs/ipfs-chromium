#include "symlink.h"

#include <gtest/gtest.h>

#include <ipfs_client/dag_block.h>
#include <ipfs_client/ipld/dag_node.h>
#include <libp2p/multi/content_identifier_codec.hpp>

using CidC = libp2p::multi::ContentIdentifierCodec;
using namespace std::literals;
namespace i = ipfs;
namespace ii = i::ipld;

TEST(SymlinkTest, fromBlock) {
  auto cid = CidC ::fromString(
      "bafybeia4wauf6z6lnnnszia6upqr5jsq7nack5nnrubf333lfg2vlabtd4");
  EXPECT_TRUE(cid.has_value());
  // The target is the last byte, \x61 aka 'a'
  i::Block b{cid.value(), "\x0a\x05\x08\x04\x12\x01\x61"};
  EXPECT_TRUE(b.type() == i::Block::Type::Symlink);
  auto node = ii::DagNode::fromBlock(b);
  EXPECT_TRUE(node);
  std::string t =
      "/ipfs/bafybeia4wauf6z6lnnnszia6upqr5jsq7nack5nnrubf333lfg2vlabtd4/c";
  auto blu = [](std::string const&) { return ii::NodePtr{}; };
  auto result = node->resolve("d/e"sv, blu, t);
  auto actual = std::get<ii::PathChange>(result);
  auto expect =
      "/ipfs/bafybeia4wauf6z6lnnnszia6upqr5jsq7nack5nnrubf333lfg2vlabtd4/"
      "a/d/e";
  EXPECT_EQ(actual.new_path, expect);
}
TEST(SymlinkTest, absolute) {
  ii::Symlink sub{
      "/ipfs/bafkreih6wk34z7qlwe2jj4ynowkh4ux6pbqnijcebcz7sd4fz3rnigxa6u"};
  ii::DagNode& t = sub;
  auto blu = [](std::string const&) -> ii::NodePtr {
    throw std::runtime_error{"Block lookup not expected."};
  };
  std::string observed_path_to_link{"/ipns/"};
  observed_path_to_link
      .append("k51qzi5uqu5dkq4jxcqvujfm2woh4p9y6inrojofxflzdnfht168zf8ynfzuu1")
      .append("symlinks/absolute_link.txt");
  auto res = t.resolve(""sv, blu, observed_path_to_link);
  EXPECT_TRUE(std::holds_alternative<ii::PathChange>(res));
  EXPECT_EQ(
      std::get<ii::PathChange>(res).new_path,
      "/ipfs/bafkreih6wk34z7qlwe2jj4ynowkh4ux6pbqnijcebcz7sd4fz3rnigxa6u");
}
TEST(SymlinkTest, rooted) {
  ii::Symlink sub{"/another/path.txt"};
  ii::DagNode& t = sub;
  auto blu = [](std::string const&) -> ii::NodePtr {
    throw std::runtime_error{"Block lookup not expected."};
  };
  std::string observed_path_to_link{"/ipns/"};
  observed_path_to_link
      .append("k51qzi5uqu5dkq4jxcqvujfm2woh4p9y6inrojofxflzdnfht168zf8ynfzuu1")
      .append("/symlinks/rooted.txt");
  auto res = t.resolve(""sv, blu, observed_path_to_link);
  EXPECT_TRUE(std::holds_alternative<ii::PathChange>(res));
  EXPECT_EQ(
      std::get<ii::PathChange>(res).new_path,
      "/ipns/k51qzi5uqu5dkq4jxcqvujfm2woh4p9y6inrojofxflzdnfht168zf8ynfzuu1/"
      "another/path.txt");
}
