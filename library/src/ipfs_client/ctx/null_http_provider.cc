#include "ipfs_client/ctx/null_http_provider.h"

using Self = ipfs::ctx::NullHttpProvider;

auto Self::SendHttpRequest(ReqDesc, OnComplete cb) const -> Canceller {
  auto hdrs = [](auto) { return std::string{}; };
  cb(500, "No HTTP networking available.", hdrs);
  return [](){};
}
