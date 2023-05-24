#include <ipfs_client/chained_requestors.h>

#include <gtest/gtest.h>

#include <ipfs_client/dag_listener.h>
#include <ipfs_client/logger.h>

namespace {
struct Listener : public ipfs::DagListener {
  std::string bytes, mime;
  void ReceiveBlockBytes(std::string_view b) { bytes.append(b); }
  void BlocksComplete(std::string mime_type) { mime = mime_type; }
  std::vector<std::pair<std::string, std::string>> not_heres, dnes;
  void NotHere(std::string_view cid, std::string_view path) {
    not_heres.emplace_back(cid, path);
  }
  void DoesNotExist(std::string_view cid, std::string_view path) {
    dnes.emplace_back(cid, path);
  }
};
enum class Result { Found, NotHere, Dne };

struct LinkReq : public ipfs::BlockRequestor {
  std::vector<std::pair<std::string, Result>> expect;
  std::string id;
  void RequestByCid(std::string cid,
                    std::shared_ptr<ipfs::DagListener> l,
                    ipfs::Priority priority) {
    EXPECT_GE(expect.size(), 1UL);
    EXPECT_EQ(expect.at(0).first, cid);
    auto resp = expect.at(0).second;
    expect.erase(expect.begin());
    switch (resp) {
      case Result::Found:
        l->ReceiveBlockBytes(id);
        l->BlocksComplete("text/plain");
        break;
      case Result::NotHere:
        l->NotHere(cid, id);
        break;
      case Result::Dne:
        l->DoesNotExist(cid, id);
    }
  }
};

struct ChainedRequestorsTest : public testing::Test {
  ipfs::ChainedRequestors under_test;
  ipfs::BlockRequestor& req() { return under_test; }
  std::shared_ptr<Listener> l = std::make_shared<Listener>();
  std::vector<std::shared_ptr<LinkReq>> chain_links;
  void add() {
    chain_links.push_back(std::make_shared<LinkReq>());
    under_test.Add(chain_links.back());
  }
};
}  // namespace

TEST_F(ChainedRequestorsTest, ctor) {}
TEST_F(ChainedRequestorsTest, add_makes_valid) {
  EXPECT_FALSE(under_test.Valid());
  add();
  EXPECT_TRUE(under_test.Valid());
}
TEST_F(ChainedRequestorsTest, prefer_first) {
  add();
  chain_links.back()->id = "first";
  chain_links.back()->expect.emplace_back("a cid", Result::Found);
  add();
  chain_links.back()->id = "second";
  chain_links.back()->expect.emplace_back("a cid", Result::Found);
  req().RequestByCid("a cid", l, 1);
  EXPECT_EQ(l->bytes, "first");
  EXPECT_EQ(l->mime, "text/plain");
  EXPECT_EQ(l->not_heres.size(), 0UL);
  EXPECT_EQ(l->dnes.size(), 0UL);
}
TEST_F(ChainedRequestorsTest, dne_still_terminates) {
  add();
  chain_links.back()->id = "first";
  chain_links.back()->expect.emplace_back("a cid", Result::Dne);
  add();
  chain_links.back()->id = "second";
  chain_links.back()->expect.emplace_back("a cid", Result::Found);
  req().RequestByCid("a cid", l, 1);
  // See the thing is, if you get a definitive, provable, DOES NOT EXIST
  // There's really no point in going on requesting the thing from other sources
  EXPECT_EQ(l->bytes, "");
  EXPECT_EQ(l->mime, "");
  EXPECT_EQ(l->not_heres.size(), 0UL);
  EXPECT_EQ(l->dnes.size(), 1UL);
  EXPECT_EQ(l->dnes.at(0).first, "a cid");
  EXPECT_EQ(l->dnes.at(0).second, "first");
}
TEST_F(ChainedRequestorsTest, not_here_moves_to_next_link_in_chain) {
  add();
  chain_links.back()->id = "first";
  chain_links.back()->expect.emplace_back("a cid", Result::NotHere);
  add();
  chain_links.back()->id = "second";
  chain_links.back()->expect.emplace_back("a cid", Result::Found);
  req().RequestByCid("a cid", l, 1);
  EXPECT_EQ(l->bytes, "second");
  EXPECT_EQ(l->mime, "text/plain");
  EXPECT_EQ(l->not_heres.size(), 0UL);
  EXPECT_EQ(l->dnes.size(), 0UL);
}
TEST_F(ChainedRequestorsTest, not_here_across_board) {
  add();
  chain_links.back()->id = "first";
  chain_links.back()->expect.emplace_back("nope", Result::NotHere);
  add();
  chain_links.back()->id = "second";
  chain_links.back()->expect.emplace_back("nope", Result::NotHere);
  ipfs::log::SetLevel(ipfs::log::Level::FATAL);
  req().RequestByCid("nope", l, 1);
  EXPECT_EQ(l->bytes, "");
  EXPECT_EQ(l->mime, "");
  EXPECT_EQ(l->not_heres.size(), 1UL);
  EXPECT_EQ(l->dnes.size(), 0UL);
  EXPECT_EQ(l->not_heres.at(0).first, "nope");

  // second because that's the last link in the chain - it's that failure that
  // lets us know we've failed overall
  EXPECT_EQ(l->not_heres.at(0).second, "second");
}
