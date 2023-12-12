#ifndef IPFS_TERMINATING_REQUESTOR_H_
#define IPFS_TERMINATING_REQUESTOR_H_

#include "requestor.h"

namespace ipfs::gw {
class TerminatingRequestor : public Requestor {
 public:
  using HandleOutcome = Requestor::HandleOutcome;
  std::string_view name() const override;
  HandleOutcome handle(RequestPtr) override;
};
}  // namespace ipfs::gw

#endif  // IPFS_TERMINATING_REQUESTOR_H_
