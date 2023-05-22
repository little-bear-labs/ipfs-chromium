#include "inter_request_state.h"

#include "gateway_requests.h"
#include "network_requestor.h"

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
    static ipfs::InterRequestState static_state({});
    return static_state;
  }
  base::SupportsUserData::Data* existing = context->GetUserData(user_data_key);
  if (existing) {
    VLOG(1) << "Re-using existing IPFS state.";
    return *static_cast<ipfs::InterRequestState*>(existing);
  }
  LOG(INFO) << "Creating new IPFS state for this browser context.";
  auto owned = std::make_unique<ipfs::InterRequestState>(context->GetPath());
  ipfs::InterRequestState* raw = owned.get();
  context->SetUserData(user_data_key, std::move(owned));
  return *raw;
}
auto ipfs::InterRequestState::serialized_caches()
    -> std::array<decltype(mem_), 2> {
  if (!mem_) {
    auto p = mem_ = std::make_shared<CacheRequestor>(
        net::CacheType::MEMORY_CACHE, *this, base::FilePath{});
    storage().AddStorageHook(
        [p](auto c, auto h, auto b) { p->Store(c, h, b); });
  }
  if (!dsk_) {
    auto p = dsk_ = std::make_shared<CacheRequestor>(net::CacheType::DISK_CACHE,
                                                     *this, disk_path_);
    storage().AddStorageHook(
        [p](auto c, auto h, auto b) { p->Store(c, h, b); });
  }
  return {mem_, dsk_};
}
auto ipfs::InterRequestState::requestor() -> BlockRequestor& {
  if (!requestor_.Valid()) {
    serialized_caches();
    requestor_.Add(mem_);
    requestor_.Add(dsk_);
    requestor_.Add(std::make_shared<NetworkRequestor>(*this));
  }
  return requestor_;
}
std::shared_ptr<ipfs::GatewayRequests> ipfs::InterRequestState::api() {
  auto existing = api_.lock();
  if (existing) {
    return existing;
  }
  auto created = std::make_shared<ipfs::GatewayRequests>(*this);
  api_ = created;
  auto t = std::time(nullptr);
  if (t - last_discovery_ > 300) {
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

ipfs::InterRequestState::InterRequestState(base::FilePath p) : disk_path_{p} {}
ipfs::InterRequestState::~InterRequestState() noexcept {}
