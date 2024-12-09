#ifndef IPFS_CHROMIUM_NULLHTTPPROVIDER_H
#define IPFS_CHROMIUM_NULLHTTPPROVIDER_H

#include "http_api.h"

namespace ipfs::ctx {
/*! "Null Object" non-implementation of HTTP requests.
 */
class NullHttpProvider : public HttpApi {
  Canceller SendHttpRequest(ReqDesc, OnComplete cb) const;
};
}  // namespace ipfs::ctx

#endif  // IPFS_CHROMIUM_NULLHTTPPROVIDER_H
