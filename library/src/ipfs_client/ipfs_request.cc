#include <ipfs_client/ipfs_request.h>

#include "log_macros.h"

using Self = ipfs::IpfsRequest;

// Self::IpfsRequest(std::string path_p)
//     : path_{path_p}, callback_([](auto&, auto&) {}) {}
Self::IpfsRequest(std::string path_p, Finisher f)
    : path_{path_p}, callback_{f} {}
void Self::till_next(std::size_t w) {
  waiting_ = w;
}
void Self::finish(ipfs::Response& r) {
  VLOG(1) << "IpfsRequest::finish(" << waiting_ << ");";
  if (waiting_) {
    if (--waiting_) {
      return;
    }
  }
  callback_(*this, r);
  // TODO - cancel other gw req pointing into this
  callback_ = [](auto& q, auto&) {
    LOG(INFO) << "IPFS request " << q.path().pop_all() << " satisfied multiply";
  };
}
bool Self::ready_after() {
  return waiting_ == 0 || 0 == --waiting_;
}
void Self::new_path(std::string_view sv) {
  path_.assign(sv);
}