#ifndef IPFS_INTER_REQUEST_STATE_H_
#define IPFS_INTER_REQUEST_STATE_H_

#include "cache_requestor.h"

#include "ipfs_client/chained_requestors.h"
#include "ipfs_client/gateways.h"
#include "ipfs_client/ipns_names.h"
#include "ipfs_client/orchestrator.h"

#include "base/supports_user_data.h"
#include "services/network/network_context.h"

namespace content {
class BrowserContext;
}

namespace ipfs {
class Scheduler;
class ChromiumIpfsContext;
class InterRequestState : public base::SupportsUserData::Data {
  Gateways gws_;
  ChainedRequestors requestor_;
  IpnsNames names_;
  std::weak_ptr<ChromiumIpfsContext> api_;
  std::time_t last_discovery_ = 0;
  std::shared_ptr<CacheRequestor> cache_;
  base::FilePath const disk_path_;
  std::shared_ptr<Orchestrator> orc_;  // TODO - map of origin to Orchestrator
  raw_ptr<network::mojom::NetworkContext> network_context_;

  std::shared_ptr<CacheRequestor>& cache();

 public:
  InterRequestState(base::FilePath);
  ~InterRequestState() noexcept override;

  Gateways& gateways() { return gws_; }
  IpnsNames& names() { return names_; }
  Scheduler& scheduler();
  std::shared_ptr<ChromiumIpfsContext> api();
  std::array<std::shared_ptr<CacheRequestor>,2> serialized_caches();
  Orchestrator& orchestrator();
  void set_network_context(raw_ptr<network::mojom::NetworkContext>);

  static InterRequestState& FromBrowserContext(content::BrowserContext*);
};
}  // namespace ipfs

#endif  // IPFS_INTER_REQUEST_STATE_H_
