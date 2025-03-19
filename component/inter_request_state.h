#ifndef IPFS_INTER_REQUEST_STATE_H_
#define IPFS_INTER_REQUEST_STATE_H_

#include "cache_requestor.h"
#include "export.h"

#include "ipfs_client/ctx/default_gateways.h"
#include "ipfs_client/ipns_names.h"
#include "ipfs_client/partition.h"

#include "base/supports_user_data.h"
#include "services/network/network_context.h"

class PrefService;

namespace content {
class BrowserContext;
}

namespace ipfs {
class Scheduler;
class COMPONENT_EXPORT(IPFS) InterRequestState
    : public base::SupportsUserData::Data {
  IpnsNames names_;
  std::shared_ptr<Client> api_;
  std::shared_ptr<CacheRequestor> cache_;
  base::FilePath const disk_path_;
  raw_ptr<network::mojom::NetworkContext> network_context_;

  std::shared_ptr<CacheRequestor>& cache();

 public:
  InterRequestState(base::FilePath, PrefService*);
  ~InterRequestState() noexcept override;

  IpnsNames& names() { return names_; }
  Scheduler& scheduler();
  std::shared_ptr<Client> api();
  std::array<std::shared_ptr<CacheRequestor>,2> serialized_caches();
  Partition& orchestrator();
  void network_context(network::mojom::NetworkContext*);
  network::mojom::NetworkContext* network_context() const;

  /*!
   * \brief Factory method
   * \param c The browser context that it will embedded with
   * \param p The preferences that control how IPFS will be used
   */
  static void CreateForBrowserContext(content::BrowserContext* c, PrefService* p);
  static InterRequestState& FromBrowserContext(content::BrowserContext*);
};
}  // namespace ipfs

#endif  // IPFS_INTER_REQUEST_STATE_H_
