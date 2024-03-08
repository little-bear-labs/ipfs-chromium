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