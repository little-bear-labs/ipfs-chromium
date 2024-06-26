#include <ipfs_client/gw/requestor.h>

#include <gtest/gtest.h>

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/gw/gateway_request_type.h>
#include <ipfs_client/ipfs_request.h>

namespace g = ipfs::gw;
using T = g::Requestor;
using RT = g::GatewayRequestType;

namespace {
struct TestRequestor : public g::Requestor {
  std::vector<g::RequestPtr> requests_sent_to_handle;
  std::vector<HandleOutcome> outcomes;
  HandleOutcome handle(g::RequestPtr r) {
    requests_sent_to_handle.push_back(r);
    auto res = outcomes.at(0);
    outcomes.erase(outcomes.begin());
    return res;
  }
  std::string name_ = "tested requestor";
  std::string_view name() const { return name_; }
  using Out = HandleOutcome;
};
using Out = TestRequestor::Out;
struct RequestorTest : public ::testing::Test {
  std::shared_ptr<TestRequestor> a = std::make_shared<TestRequestor>();
  std::shared_ptr<TestRequestor> b = std::make_shared<TestRequestor>();
  std::shared_ptr<g::GatewayRequest> req_ =
      std::make_shared<g::GatewayRequest>();
  RequestorTest() {
    req_->type = RT::Block;
    req_->dependent =
        std::make_shared<ipfs::IpfsRequest>("", [](auto&, auto&) {});
  }
};
}  // namespace

TEST_F(RequestorTest, can_ctor) {}

TEST_F(RequestorTest, nothandled_continues) {
  a->or_else(b);
  a->outcomes.push_back(Out::NOT_HANDLED);
  b->outcomes.push_back(Out::PENDING);
  a->request(req_);
  EXPECT_EQ(a->requests_sent_to_handle.size(), 1);
  EXPECT_EQ(b->requests_sent_to_handle.size(), 1);
  EXPECT_TRUE(a->requests_sent_to_handle.at(0) == req_);
  EXPECT_TRUE(b->requests_sent_to_handle.at(0) == req_);
}

TEST_F(RequestorTest, done_stops_request) {
  a->or_else(b);
  a->outcomes.push_back(Out::DONE);
  b->outcomes.push_back(Out::PENDING);
  a->request(req_);
  EXPECT_EQ(a->requests_sent_to_handle.size(), 1);
  EXPECT_EQ(b->requests_sent_to_handle.size(), 0);
  EXPECT_TRUE(a->requests_sent_to_handle.at(0) == req_);
}

