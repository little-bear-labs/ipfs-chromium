#ifndef IPFS_INLINE_REQUEST_HANDLER_H_
#define IPFS_INLINE_REQUEST_HANDLER_H_

#include "requestor.h"

namespace ipfs::gw {
/*! Handles requests for inline requests,
 *  i.e. /ipfs/ with hash = identity
 *  @note The CID contains the response
 */
class InlineRequestHandler final : public Requestor {
 public:
  /*! @param request The request to try handling
   *  @return DONE (after responding) if it was an inline request
   *    NOT_HANDLED otherwise
   */
  HandleOutcome handle(RequestPtr request) override;
  std::string_view name() const override;
};
}  // namespace ipfs::gw

#endif  // IPFS_INLINE_REQUEST_HANDLER_H_
