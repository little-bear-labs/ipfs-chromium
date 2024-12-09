#ifndef IPFS_TERMINATING_REQUESTOR_H_
#define IPFS_TERMINATING_REQUESTOR_H_

#include "requestor.h"

namespace ipfs::gw {
/*! Intended to appear at the end of the decorator list,
 *  This requestor doesn't issue requests, but just wraps up loose ends.
 */
class TerminatingRequestor : public Requestor {
 public:
  std::string_view name() const override;
  Requestor::HandleOutcome handle(RequestPtr) override;
};
}  // namespace ipfs::gw

#endif  // IPFS_TERMINATING_REQUESTOR_H_
