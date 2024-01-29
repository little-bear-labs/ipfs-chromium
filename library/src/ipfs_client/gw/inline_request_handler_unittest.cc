#include <ipfs_client/gw/inline_request_handler.h>

#include <mock_api.h>

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/gw/terminating_requestor.h>
#include <ipfs_client/partition.h>

namespace i = ipfs;
namespace ig = i::gw;
namespace pm = libp2p::multi;
using namespace std::literals;
using T = ig::InlineRequestHandler;
using ipfs::Cid;

TEST(InlineRequestHanlder, bluesky) {
  T t;
  auto api = std::make_shared<MockApi>();
  auto rtor = std::make_shared<ig::TerminatingRequestor>();
  auto orc = api->with(rtor).partition({});
  auto r = std::make_shared<ig::GatewayRequest>();
  r->type = ig::Type::Identity;
  r->orchestrator(orc);
  i::MultiHash h(i::HashType::IDENTITY, i::as_bytes("abc"sv));
  r->cid = Cid(i::MultiCodec::RAW, h);
  auto res = t.handle(r);
  EXPECT_EQ(static_cast<char>(res), 'D');
}