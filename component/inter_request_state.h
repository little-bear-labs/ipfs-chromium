#ifndef IPFS_INTER_REQUEST_STATE_H_
#define IPFS_INTER_REQUEST_STATE_H_

#include "ipfs_client/block_storage.h"
#include "ipfs_client/gateways.h"
#include "ipfs_client/ipns_names.h"

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
  IpnsNames names_;
  std::weak_ptr<GatewayRequests> api_;
  std::time_t last_discovery_ = 0;

 public:
  InterRequestState();
  ~InterRequestState() noexcept override;

  Gateways& gateways() { return gws_; }
  BlockStorage& storage() { return storage_; }
  IpnsNames& names() { return names_; }
  Scheduler& scheduler();
  std::shared_ptr<GatewayRequests> api();

  static InterRequestState& FromBrowserContext(content::BrowserContext*);
};
}  // namespace ipfs

#endif  // IPFS_INTER_REQUEST_STATE_H_
