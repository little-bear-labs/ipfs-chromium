#include <ipfs_client/ipfs_request.h>

#include <ipfs_client/response.h>

#include "log_macros.h"

#include <memory>

using Self = ipfs::IpfsRequest;

// Self::IpfsRequest(std::string path_p)
//     : path_{path_p}, callback_([](auto&, auto&) {}) {}
Self::IpfsRequest(std::string path_p, Finisher f)
    : path_{path_p}, callback_{f} {}

std::shared_ptr<Self> Self::fromUrl(std::string url, ipfs::IpfsRequest::Finisher f) {
  url.erase(4UL, 2UL );
  url.insert(0UL, 1UL, '/');
  return std::make_shared<Self>(std::move(url), std::move(f));
}

void Self::finish(ipfs::Response& r) {
  callback_(*this, r);
  // TODO - cancel other gw req pointing into this
  callback_ = [](auto& q, auto&) {
    VLOG(2) << "IPFS request " << q.path().pop_all() << " satisfied multiply";
  };
}
void Self::new_path(std::string_view sv) {
  path_.assign(sv);
}