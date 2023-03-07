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
class InterRequestState : public base::SupportsUserData::Data {
  Gateways gws_;
  BlockStorage storage_;
  IpnsNames names_;

 public:
  InterRequestState();
  ~InterRequestState() noexcept override;

  Gateways& gateways() { return gws_; }
  BlockStorage& storage() { return storage_; }
  IpnsNames& names() { return names_; }

  static InterRequestState& FromBrowserContext(content::BrowserContext*);
};
}  // namespace ipfs

#endif  // IPFS_INTER_REQUEST_STATE_H_
