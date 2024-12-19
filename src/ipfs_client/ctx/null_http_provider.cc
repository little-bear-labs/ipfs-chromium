#include "ipfs_client/ctx/null_http_provider.h"
#include <string>

using Self = ipfs::ctx::NullHttpProvider;

namespace {
constexpr auto INTERNAL_SERVER_ERROR_STATUS = 500;
}

auto Self::SendHttpRequest(ReqDesc, OnComplete callback) const -> Canceller {
  auto hdrs = [](auto) { return std::string{}; };
  callback(INTERNAL_SERVER_ERROR_STATUS, "No HTTP networking available.", hdrs);
  return [](){};
}
