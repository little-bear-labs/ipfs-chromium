#include "multi_gateway_requestor.h"
#include "ipfs_client/gw/gateway_request_type.h"

#include <mock_api.h>

#include <ipfs_client/gw/gateway_request.h>

namespace {
ig::RequestPtr block_req() {
  auto rv = std::make_shared<ig::GatewayRequest>();
  rv->type = RT::Block;
  rv->main_param =
      "bafybeid4dzlxm6h4r6kfvx6jp6vj4nteplmbve224lx2s3lgjubyufsuo4";
  return rv;
}
}  // namespace

TEST(MultiGatewayRequestor, FailsWithoutApi) {
  ig::MultiGatewayRequestor t;
  auto o = t.handle(block_req());
  EXPECT_TRUE(o == ig::Requestor::HandleOutcome::DONE);
}
TEST(MultiGatewayRequestor, FailsIfNotHttp) {
  ig::MultiGatewayRequestor t;
  t.api(std::make_shared<MockApi>());
  auto r = block_req();
  r->type = RT::Identity;
  auto o = t.handle(r);
  EXPECT_TRUE(o == ig::Requestor::HandleOutcome::NOT_HANDLED);
}
TEST(MultiGatewayRequestor, PendingIfHttp) {
  ig::MultiGatewayRequestor t;
  t.api(std::make_shared<MockApi>());
  auto r = block_req();
  r->type = RT::Block;
  auto o = t.handle(r);
  EXPECT_TRUE(o == ig::Requestor::HandleOutcome::PENDING);
}
TEST(MultiGatewayRequestor, SendsRequestIfGatewayThereWithAnHttpRequest) {
  auto t = std::make_shared<ig::MultiGatewayRequestor>();
  auto api = std::make_shared<MockApi>();
  api->gw_->index_gettable_gateways.emplace_back("telnet://::1/", 9);
  t->api(api);
  auto r = block_req();
  r->type = RT::Block;
  auto o = t->handle(r);
  EXPECT_TRUE(o == ig::Requestor::HandleOutcome::PENDING);
  EXPECT_EQ(api->h_->requests_sent.size(),1UL);
  auto& rs = api->h_->requests_sent.at(0);
  auto url_sent_to = rs.first.url;
  EXPECT_LT(url_sent_to.find("bafybeid4dzlxm6h"),999UL) << url_sent_to;
}
