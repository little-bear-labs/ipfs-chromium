#ifndef IPFS_INLINE_REQUEST_HANDLER_H_
#define IPFS_INLINE_REQUEST_HANDLER_H_

#include "requestor.h"

namespace ipfs::gw {
class InlineRequestHandler final : public Requestor {
  HandleOutcome handle(RequestPtr) override;
  std::string_view name() const override;
};
}  // namespace ipfs::gw

#endif  // IPFS_INLINE_REQUEST_HANDLER_H_
