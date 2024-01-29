#ifndef IPFS_CHROMIUM_NULLHTTPPROVIDER_H
#define IPFS_CHROMIUM_NULLHTTPPROVIDER_H

#include "http_api.h"

namespace ipfs::ctx {
class NullHttpProvider : public HttpApi {
  void SendHttpRequest(HttpRequestDescription, HttpCompleteCallback cb) const;
};
}  // namespace ipfs::ctx

#endif  // IPFS_CHROMIUM_NULLHTTPPROVIDER_H
