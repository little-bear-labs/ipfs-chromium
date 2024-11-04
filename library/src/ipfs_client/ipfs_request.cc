#include <ipfs_client/ipfs_request.h>

#include <ipfs_client/response.h>

#include "ipfs_client/response_semantic.h"

#include <memory>
#include <utility>
#include <string>
#include <string_view>

using Self = ipfs::IpfsRequest;

Self::IpfsRequest(std::string path_p, Finisher fin)
    : path_{std::move(path_p)}, callback_{std::move(fin)}, semantic_{ResponseSemantic::Http} {}

Self::~IpfsRequest() noexcept {
  for (auto& cleanup : cleanups_) {
#if __cpp_exceptions
    try {
        cleanup();
    } catch (...) {}
#else
    cleanup();
#endif
  }
}

auto Self::fromUrl(std::string url, ipfs::IpfsRequest::Finisher f) -> std::shared_ptr<Self> {
  url.erase(4UL, 2UL );
  url.insert(0UL, 1UL, '/');
  return std::make_shared<Self>(std::move(url), std::move(f));
}

void Self::finish(ipfs::Response& r) {
  if (callback_) {
    callback_(*this, r);
    callback_ = {};
  }
  for (auto& cleanup : cleanups_) {
    cleanup();
  }
  cleanups_.clear();
}
void Self::new_path(std::string_view sv) {
  path_.assign(sv);
}
auto Self::done() const -> bool {
  return !callback_;
}
auto Self::semantic(std::string_view sem) -> Self& {
  if (sem.find("isting") == 1UL) {
    semantic_ = ResponseSemantic::Listing;
  } else {
    semantic_ = ResponseSemantic::Http;
  }
  return *this;
}
void Self::to_cleanup(Cleanup c) {
  cleanups_.push_back(std::move(c));
}
