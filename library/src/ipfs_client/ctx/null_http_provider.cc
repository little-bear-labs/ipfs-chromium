#include "ipfs_client/ctx/null_http_provider.h"

using Self = ipfs::ctx::NullHttpProvider;

void Self::SendHttpRequest(HttpRequestDescription,
                           HttpCompleteCallback cb) const {
  auto hdrs = [](auto) { return std::string{}; };
  cb(500, "No HTTP networking available.", hdrs);
}