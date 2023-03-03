#include "inter_request_state.h"

#include "base/logging.h"
#include "content/public/browser/browser_context.h"

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

ipfs::InterRequestState::InterRequestState() {}
ipfs::InterRequestState::~InterRequestState() noexcept {}