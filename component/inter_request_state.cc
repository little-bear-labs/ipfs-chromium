#include "inter_request_state.h"

#include "chromium_ipfs_context.h"
#include "network_requestor.h"

#include "base/logging.h"
#include "content/public/browser/browser_context.h"

#include <ipfs_client/gw/default_requestor.h>

#include <ipfs_client/dag_listener.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/response.h>

using Self = ipfs::InterRequestState;

namespace {
constexpr char user_data_key[] = "ipfs_request_userdata";
}

auto Self::FromBrowserContext(content::BrowserContext* context)
    -> InterRequestState& {
  if (!context) {
    LOG(WARNING) << "No browser context! Using a default IPFS state.";
    static ipfs::InterRequestState static_state({});
    return static_state;
  }
  base::SupportsUserData::Data* existing = context->GetUserData(user_data_key);
  if (existing) {
    VLOG(2) << "Re-using existing IPFS state.";
    return *static_cast<ipfs::InterRequestState*>(existing);
  }
  VLOG(2) << "Creating new IPFS state for this browser context.";
  auto owned = std::make_unique<ipfs::InterRequestState>(context->GetPath());
  ipfs::InterRequestState* raw = owned.get();
  context->SetUserData(user_data_key, std::move(owned));
  return *raw;
}
std::shared_ptr<ipfs::ChromiumIpfsContext> Self::api() {
  auto existing = api_.lock();
  if (existing) {
    return existing;
  }
  auto created =
      std::make_shared<ipfs::ChromiumIpfsContext>(*this, network_context_);
  api_ = created;
  auto t = std::time(nullptr);
  if (t - last_discovery_ > 300) {
    created->Discover([this](auto v) { gws_.AddGateways(v); });
    last_discovery_ = t;
  }
  return created;
}
auto Self::cache() -> std::shared_ptr<CacheRequestor>& {
  if (!cache_) {
    cache_ = std::make_shared<CacheRequestor>(*this, disk_path_);
  }
  return cache_;
}
auto Self::orchestrator() -> Orchestrator& {
  if (!orc_) {
    auto rtor =
        gw::default_requestor(gateways().GenerateList(), cache(), api());
    orc_ = std::make_shared<Orchestrator>(rtor, api());
  }
  return *orc_;
}
void Self::set_network_context(raw_ptr<network::mojom::NetworkContext> val) {
  network_context_ = val;
}
Self::InterRequestState(base::FilePath p) : disk_path_{p} {}
Self::~InterRequestState() noexcept {}
