#pragma once

#include <ipfs_client/ctx/http_api.h>

namespace {
  struct MockHttpProvider final : public ipfs::ctx::HttpApi {
   mutable std::vector<std::pair<ReqDesc, OnComplete>> requests_sent;
   mutable std::vector<std::size_t> requests_cancelled;
   Canceller SendHttpRequest(ReqDesc d, OnComplete cb) const {
    auto i = requests_sent.size();
    requests_sent.emplace_back(d, cb);
    return [this,i](){requests_cancelled.push_back(i);};
   }
  };
}

