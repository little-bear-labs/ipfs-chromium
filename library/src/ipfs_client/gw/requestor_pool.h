#ifndef IPFS_REQUESTOR_POOL_H_
#define IPFS_REQUESTOR_POOL_H_

#include <ipfs_client/gw/requestor.h>

#include <vocab/flat_mapset.h>

#include <ctime>
#include <queue>
#include <vector>

namespace ipfs::gw {
class RequestorPool : public Requestor {
  std::string_view name() const override;
  HandleOutcome handle(RequestPtr) override;

  std::vector<std::shared_ptr<Requestor>> pool_;
  struct Waiting {
    RequestPtr req;
    std::size_t at_idx;
    std::time_t when;
  };
  std::queue<Waiting> waiting_;

  HandleOutcome check(Waiting);
  
 public:
  RequestorPool& add(std::shared_ptr<Requestor>);
};
}  // namespace ipfs::gw

#endif  // IPFS_REQUESTOR_POOL_H_
