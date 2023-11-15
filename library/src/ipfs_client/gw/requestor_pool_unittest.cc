#include <ipfs_client/gw/requestor_pool.h>

#include <ipfs_client/gw/gateway_request.h>

#include <mock_api.h>
#include <mock_requestor.h>

namespace {
struct RequestorPoolTest : public ::testing::Test {
  std::vector<std::shared_ptr<MockRequestor>> members;
  std::shared_ptr<MockApi> api = std::make_shared<MockApi>();
  std::shared_ptr<ig::RequestorPool> tested =
      std::make_shared<ig::RequestorPool>();
  std::shared_ptr<ig::GatewayRequest> req =
      ig::GatewayRequest::fromIpfsPath("/ipns/ipfs.io"sv);
  void add() {
    auto p = std::make_shared<MockRequestor>();
    members.push_back(p);
    tested->add(p);
  }
  void set_api() {
    auto p = std::make_shared<MockRequestor>();
    p->api(api);
    p->or_else(tested);
  }
};
}  // namespace

TEST_F(RequestorPoolTest, add_with_api_sets_member_api) {
  add();
  EXPECT_EQ(members.size(), 1U);
  set_api();
  add();
  EXPECT_EQ(members.size(), 2U);
  EXPECT_TRUE(members.at(0));
  EXPECT_TRUE(members.at(1));
  EXPECT_FALSE(members.at(0)->api());
  EXPECT_TRUE(members.at(1)->api());
}
TEST_F(RequestorPoolTest, pending_doesnt_stop_parallel_requests) {
  add();
  add();
  members.at(0)->outcomes.push_back(MockRequestor::O::PENDING);
  members.at(1)->outcomes.push_back(MockRequestor::O::PENDING);
  tested->request(req);
}