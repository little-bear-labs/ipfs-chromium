#include "inter_request_state.h"

#include "gateway_requests.h"

#include "base/logging.h"
#include "content/public/browser/browser_context.h"

#include <ipfs_client/scheduler.h>

namespace {
constexpr char user_data_key[] = "ipfs_request_userdata";
}

auto ipfs::InterRequestState::FromBrowserContext(
    content::BrowserContext* context) -> InterRequestState& {
  if (!context) {
    LOG(WARNING) << "No browser context! Using a default IPFS state.";
    static ipfs::InterRequestState static_state;
    return static_state;
  }
  base::SupportsUserData::Data* existing = context->GetUserData(user_data_key);
  if (existing) {
    LOG(INFO) << "Re-using existing IPFS state.";
    return *static_cast<ipfs::InterRequestState*>(existing);
  }
  LOG(INFO) << "Creating new IPFS state for this browser context.";
  auto owned = std::make_unique<ipfs::InterRequestState>();
  ipfs::InterRequestState* raw = owned.get();
  context->SetUserData(user_data_key, std::move(owned));
  return *raw;
}
std::shared_ptr<ipfs::GatewayRequests> ipfs::InterRequestState::api() {
  auto existing = api_.lock();
  if (existing) {
    return existing;
  }
  /*
 if (api_) {
   return api_;
 }*/
  auto created = std::make_shared<ipfs::GatewayRequests>(*this);
  api_ = created;
  auto t = std::time(nullptr);
  if (t - last_discovery_ > 9) {
    created->Discover([this](auto v) { gws_.AddGateways(v); });
    last_discovery_ = t;
  }
  return created;
}
auto ipfs::InterRequestState::scheduler() -> Scheduler& {
  auto api = api_.lock();
  DCHECK(api);
  return api->scheduler();
}

ipfs::InterRequestState::InterRequestState() {}
ipfs::InterRequestState::~InterRequestState() noexcept {}