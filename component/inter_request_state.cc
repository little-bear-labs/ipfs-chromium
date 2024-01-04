#include "inter_request_state.h"

#include "chromium_ipfs_context.h"
#include "preferences.h"

#include <base/logging.h>
#include "content/public/browser/browser_context.h"

#include <ipfs_client/gateways.h>
#include <ipfs_client/gw/default_requestor.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/response.h>

using Self = ipfs::InterRequestState;

namespace {
constexpr char user_data_key[] = "ipfs_request_userdata";
}

void Self::CreateForBrowserContext(content::BrowserContext* c, PrefService* p) {
  DCHECK(c);
  DCHECK(p);
  LOG(INFO) << "Creating new IPFS state for this browser context.";
  auto owned = std::make_unique<ipfs::InterRequestState>(c->GetPath(), p);
  c->SetUserData(user_data_key, std::move(owned));
}
auto Self::FromBrowserContext(content::BrowserContext* context)
    -> InterRequestState& {
  if (!context) {
    LOG(WARNING) << "No browser context! Using a default IPFS state.";
    static ipfs::InterRequestState static_state({}, {});
    return static_state;
  }
  base::SupportsUserData::Data* existing = context->GetUserData(user_data_key);
  if (existing) {
    return *static_cast<ipfs::InterRequestState*>(existing);
  } else {
    LOG(ERROR) << "Browser context has no IPFS state! It must be set earlier!";
    static ipfs::InterRequestState static_state({}, {});
    return static_state;
  }
}
std::shared_ptr<ipfs::ChromiumIpfsContext> Self::api() {
  return api_;
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
        gw::default_requestor(Gateways::DefaultGateways(), cache(), api());
    orc_ = std::make_shared<Orchestrator>(rtor, api());
  }
  return *orc_;
}
void Self::network_context(network::mojom::NetworkContext* val) {
  network_context_ = val;
}
network::mojom::NetworkContext* Self::network_context() const {
  return network_context_;
}
Self::InterRequestState(base::FilePath p, PrefService* prefs)
    : api_{std::make_shared<ChromiumIpfsContext>(*this, prefs)}, disk_path_{p} {
  DCHECK(prefs);
}
Self::~InterRequestState() noexcept {
  network_context_ = nullptr;
  cache_.reset();
}
