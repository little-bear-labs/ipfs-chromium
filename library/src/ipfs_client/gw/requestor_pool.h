#ifndef IPFS_REQUESTOR_POOL_H_
#define IPFS_REQUESTOR_POOL_H_

#include <ipfs_client/gw/requestor.h>

#include <vocab/flat_mapset.h>

#include <queue>
#include <vector>

namespace ipfs::gw {
class RequestorPool : public Requestor {
  std::string_view name() const override;
  HandleOutcome handle(RequestPtr) override;

  HandleOutcome check(RequestPtr, std::size_t);

  std::vector<std::shared_ptr<Requestor>> pool;
  std::queue<std::pair<RequestPtr, std::size_t>> waiting;

 public:
  RequestorPool& add(std::shared_ptr<Requestor>);
};
}  // namespace ipfs::gw

#endif  // IPFS_REQUESTOR_POOL_H_
