#ifndef IPFS_DEDUPLICATING_REQUESTOR_H_
#define IPFS_DEDUPLICATING_REQUESTOR_H_

#include <ipfs_client/gw/requestor.h>

#include <ipfs_client/context_api.h>

#include <map>

namespace ipfs::gw {
class DeduplicatingRequestor final : public Requestor {
  std::string_view name() const override;
  HandleOutcome handle(RequestPtr) override;

  std::map<HttpRequestDescription, std::weak_ptr<GatewayRequest>> seen_;
};
}  // namespace ipfs::gw

#endif  // IPFS_DEDUPLICATING_REQUESTOR_H_
