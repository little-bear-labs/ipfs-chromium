#ifndef IPFS_TEST_REQUESTOR_H_
#define IPFS_TEST_REQUESTOR_H_

#include <ipfs_client/context_api.h>
#include <ipfs_client/gw/requestor.h>

namespace {
struct MockRequestor final : public ipfs::gw::Requestor {
  using RequestPtr = ipfs::gw::RequestPtr;
  using ContextApi = ipfs::ContextApi;
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
