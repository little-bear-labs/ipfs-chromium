#include <ipfs_client/gw/block_request_splitter.h>

#include <gtest/gtest.h>

#include <ipfs_client/gw/gateway_request.h>

namespace g = ipfs::gw;

using Tested = g::BlockRequestSplitter;
using Req = g::GatewayRequest;

namespace {
struct Recording : public g::Requestor {
  std::string_view name() const { return "recording requestor for test"; }
  std::vector<g::RequestPtr> requests_received;
  HandleOutcome handle(g::RequestPtr r) {
    requests_received.push_back(r);
    return HandleOutcome::DONE;
  }
};
}  // namespace

TEST(BlockRequestSplitterTest, split2three) {
  Tested tested;
  auto rec = std::make_shared<Recording>();
  tested.or_else(rec);
  auto req = std::make_shared<Req>();
  req->type = g::Type::Car;
  req->main_param = "cid";
  req->path = "path";
  req->parallel = 123;
  tested.request(req);
  EXPECT_EQ(rec->requests_received.size(), 3U);
  EXPECT_TRUE(rec->requests_received.at(0)->type == g::Type::Block) <<
static_cast<int>(rec->requests_received.at(0)->type);
  EXPECT_EQ(rec->requests_received.at(0)->main_param, "cid");
  EXPECT_EQ(rec->requests_received.at(0)->path, "");

  EXPECT_TRUE(rec->requests_received.at(1)->type == g::Type::Providers)
      << static_cast<int>(rec->requests_received.at(2)->type);
  EXPECT_EQ(rec->requests_received.at(1)->main_param, "cid");
  EXPECT_EQ(rec->requests_received.at(1)->path, "");

  EXPECT_TRUE(rec->requests_received.at(2)->type == g::Type::Car)
      << static_cast<int>(rec->requests_received.at(2)->type);
  EXPECT_EQ(rec->requests_received.at(2)->main_param, "cid");
  EXPECT_EQ(rec->requests_received.at(2)->path, "path");
}
