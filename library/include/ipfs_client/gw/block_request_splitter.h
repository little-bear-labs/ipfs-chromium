#ifndef IPFS_BLOCK_REQUEST_SPLITTER_H_
#define IPFS_BLOCK_REQUEST_SPLITTER_H_

#include "requestor.h"

#include <array>
#include <string>

namespace ipfs {
class Client;
}

namespace ipfs::gw {
/*! A request decorator of sorts that passes through non-CAR requests
 *  But for a CAR request will output both the original request and also a block request for the root
 */
class BlockRequestSplitter final : public Requestor {
  using RequestQ = std::array<std::string, 32>;
  RequestQ recent_provider_requests;
  std::size_t old_provider_request = 0UL;
  HandleOutcome handle(RequestPtr) override;
  std::string_view name() const override;
};
}  // namespace ipfs::gw

#endif  // IPFS_BLOCK_REQUEST_SPLITTER_H_
