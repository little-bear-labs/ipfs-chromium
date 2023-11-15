#ifndef IPFS_INTER_REQUEST_STATE_H_
#define IPFS_INTER_REQUEST_STATE_H_

#include "cache_requestor.h"

#include "ipfs_client/block_storage.h"
#include "ipfs_client/chained_requestors.h"
#include "ipfs_client/gateways.h"
#include "ipfs_client/ipns_names.h"
#include "ipfs_client/orchestrator.h"

#include "base/supports_user_data.h"

namespace content {
class BrowserContext;
}

namespace ipfs {
class Scheduler;
class GatewayRequests;
class InterRequestState : public base::SupportsUserData::Data {
  Gateways gws_;
  BlockStorage storage_;
  ChainedRequestors requestor_;
  IpnsNames names_;
  std::weak_ptr<GatewayRequests> api_;
  std::time_t last_discovery_ = 0;
  std::shared_ptr<CacheRequestor> mem_, dsk_;
  base::FilePath const disk_path_;
  std::shared_ptr<Orchestrator> orc_;  // TODO - map of origin to Orchestrator

 public:
  InterRequestState(base::FilePath);
  ~InterRequestState() noexcept override;

  Gateways& gateways() { return gws_; }
  BlockStorage& storage() { return storage_; }
  BlockRequestor& requestor();
  IpnsNames& names() { return names_; }
  Scheduler& scheduler();
  std::shared_ptr<GatewayRequests> api();
  std::array<std::shared_ptr<CacheRequestor>,2> serialized_caches();
  Orchestrator& orchestrator();

  static InterRequestState& FromBrowserContext(content::BrowserContext*);
};
}  // namespace ipfs

#endif  // IPFS_INTER_REQUEST_STATE_H_
