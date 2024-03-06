#include <ipfs_client/gw/terminating_requestor.h>

#include <gtest/gtest.h>

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipfs_request.h>
#include "ipfs_client/gw/gateway_request_type.h"

namespace i = ipfs;
namespace ig = i::gw;
using T = ig::TerminatingRequestor;
using RT = ig::GatewayRequestType;

TEST(TerminatingRequestorTest, ZombieIsDone) {
  auto req = std::make_shared<ig::GatewayRequest>();
  req->type = RT::Zombie;
  T tested;
  EXPECT_TRUE(tested.handle(req) == T::HandleOutcome::DONE);
}
TEST(TerminatingRequestorTest, BeingHandledInParallel) {
  auto req = std::make_shared<ig::GatewayRequest>();
  req->type = RT::Block;
  req->dependent = std::make_shared<i::IpfsRequest>("", [](auto&, auto&) {});
  req->parallel = 9;
  T tested;
  EXPECT_TRUE(tested.handle(req) == T::HandleOutcome::PENDING);
}
TEST(TerminatingRequestorTest, FreshIsTerminated) {
  auto req = std::make_shared<ig::GatewayRequest>();
  bool finished = false;
  auto finisher = [&finished](auto const&, auto const&) { finished = true; };
  req->dependent = std::make_shared<i::IpfsRequest>("Yo! :)", finisher);
  T tested;
  EXPECT_TRUE(tested.handle(req) == T::HandleOutcome::DONE);
}
