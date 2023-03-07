#include "inter_request_state.h"

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
std::shared_ptr<ipfs::Scheduler> ipfs::InterRequestState::scheduler() {
  //  auto existing = existing_scheduler_.lock();
  //  if (existing) {
  //    return existing;
  //  }
  auto created = std::make_shared<ipfs::Scheduler>(gateways().GenerateList());
  //  existing_scheduler_ = created;
  return created;
}

ipfs::InterRequestState::InterRequestState() {}
ipfs::InterRequestState::~InterRequestState() noexcept {}