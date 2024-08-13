#ifndef IPFS_TEST_REQUESTOR_H_
#define IPFS_TEST_REQUESTOR_H_

#include <ipfs_client/client.h>
#include <ipfs_client/gw/requestor.h>

#include <gtest/gtest.h>

namespace i = ipfs;
namespace ig = i::gw;

namespace {
struct MockRequestor final : public ig::Requestor {
  using RequestPtr = ig::RequestPtr;
  using ContextApi = i::Client;
  using O = MockRequestor::HandleOutcome;
  std::vector<RequestPtr> requests;
  std::vector<HandleOutcome> outcomes;
  HandleOutcome handle(RequestPtr p) {
    requests.push_back(p);
    auto result = outcomes.at(0);
    outcomes.erase(outcomes.begin());
    return result;
  }
  std::string name_ = "MockRequestor";
  std::string_view name() const { return name_; }
  std::shared_ptr<ContextApi> api() const { return api_; }
  void api(std::shared_ptr<ContextApi> a) { api_ = a; }
};
}  // namespace

#endif  // IPFS_TEST_REQUESTOR_H_
