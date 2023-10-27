#include <ipfs_client/ipfs_request.h>

#include "log_macros.h"

using Self = ipfs::IpfsRequest;

// Self::IpfsRequest(std::string path_p)
//     : path_{path_p}, callback_([](auto&, auto&) {}) {}
Self::IpfsRequest(std::string path_p, Finisher f)
    : path_{path_p}, callback_{f} {}

void Self::finish(ipfs::Response& r) {
  LOG(INFO) << "IpfsRequest::finish(...);";
  callback_(*this, r);
  // TODO - cancel other gw req pointing into this
  callback_ = [](auto& q, auto&) {
    LOG(INFO) << "IPFS request " << q.path().pop_all() << " satisfied multiply";
  };
}
