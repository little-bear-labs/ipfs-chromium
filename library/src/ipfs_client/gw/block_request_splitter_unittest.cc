#include <ipfs_client/gw/block_request_splitter.h>

#include <mock_api.h>

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipfs_request.h>
#include "ipfs_client/gw/gateway_request_type.h"

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
  tested.api(std::make_shared<MockApi>());
  auto rec = std::make_shared<Recording>();
  tested.or_else(rec);
  auto req = std::make_shared<Req>();
  req->type = RT::Car;
  req->root_component( "cid");
  req->path = "path";
  req->parallel = 123;
  req->affinity = __func__;
  req->dependent = std::make_shared<ipfs::IpfsRequest>("", [](auto&, auto&) {});
  tested.request(req);
  EXPECT_EQ(rec->requests_received.size(), 3U);
  EXPECT_TRUE(rec->requests_received.at(0)->type == RT::Block)
      << static_cast<int>(rec->requests_received.at(0)->type);
  EXPECT_EQ(rec->requests_received.at(0)->root_component(), "cid");
  EXPECT_EQ(rec->requests_received.at(0)->path, "");

  EXPECT_TRUE(rec->requests_received.at(1)->type == RT::Providers)
      << static_cast<int>(rec->requests_received.at(2)->type);
  EXPECT_EQ(rec->requests_received.at(1)->root_component(), "cid");
  EXPECT_EQ(rec->requests_received.at(1)->path, "");

  EXPECT_TRUE(rec->requests_received.at(2)->type == RT::Car)
      << static_cast<int>(rec->requests_received.at(2)->type);
  EXPECT_EQ(rec->requests_received.at(2)->root_component(), "cid");
  EXPECT_EQ(rec->requests_received.at(2)->path, "path");
}
