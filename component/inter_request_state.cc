#include "inter_request_state.h"

#include "chromium_ipfs_context.h"
#include "json_parser_adapter.h"
#include "preferences.h"

#include <base/logging.h>
#include <content/public/browser/browser_context.h>
#include <content/browser/child_process_security_policy_impl.h>
#include <third_party/blink/renderer/platform/weborigin/scheme_registry.h>

#include <ipfs_client/gw/default_requestor.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/response.h>
#include "ipfs_client/ctx/default_gateways.h"

using Self = ipfs::InterRequestState;

namespace {
constexpr char user_data_key[] = "ipfs_request_userdata";
}

void Self::CreateForBrowserContext(content::BrowserContext* c, PrefService* p) {
  DCHECK(c);
  DCHECK(p);
  auto owned = std::make_unique<ipfs::InterRequestState>(c->GetPath(), p);
  c->SetUserData(user_data_key, std::move(owned));
  auto* cpsp = content::ChildProcessSecurityPolicy::GetInstance();
  for (std::string scheme : {"ipfs", "ipns"}) {
    cpsp->RegisterWebSafeScheme(scheme);
  }
}
auto Self::FromBrowserContext(content::BrowserContext* context)
    -> InterRequestState& {
  DCHECK(context);
  auto* cpsp = content::ChildProcessSecurityPolicy::GetInstance();
  for (std::string scheme : {"ipfs", "ipns"}) {
    if (!(cpsp->IsWebSafeScheme(scheme))) {
      cpsp->RegisterWebSafeScheme(scheme);
    }
  }
  // __attribute__((no_destroy))
  // static ipfs::InterRequestState static_state({}, {});
  // if (!context) {
  //   LOG(WARNING) << "No browser context! Using a default IPFS state.";
  //   return static_state;
  // }
  base::SupportsUserData::Data* existing = context->GetUserData(user_data_key);
  if (existing) {
    return *static_cast<ipfs::InterRequestState*>(existing);
  } else {
    LOG(FATAL) << "Browser context has no IPFS state! It must be set earlier!";
    // return static_state;
  }
}
std::shared_ptr<ipfs::Client> Self::api() {
  return api_;
}
auto Self::cache() -> std::shared_ptr<CacheRequestor>& {
  if (!cache_) {
    cache_ = std::make_shared<CacheRequestor>(*this, disk_path_);
  }
  return cache_;
}
auto Self::orchestrator() -> Partition& {
  if (!cache_) {
    auto rtor = gw::default_requestor(cache(), api());
    api()->with(rtor);
  }
  // TODO - use origin
  return *api()->partition({});
}
void Self::network_context(network::mojom::NetworkContext* val) {
  network_context_ = val;
}
network::mojom::NetworkContext* Self::network_context() const {
  return network_context_;
}
Self::InterRequestState(base::FilePath p, PrefService* prefs)
    : api_{CreateContext(*this, prefs)}, disk_path_{p} {
  api_->with(std::make_unique<JsonParserAdapter>());
  DCHECK(prefs);
}
Self::~InterRequestState() noexcept {
  network_context_ = nullptr;
  cache_.reset();
}
