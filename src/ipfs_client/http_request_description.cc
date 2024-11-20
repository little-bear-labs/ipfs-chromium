#include <ipfs_client/http_request_description.h>

using Self = ipfs::HttpRequestDescription;

auto Self::operator==(HttpRequestDescription const& r) const -> bool {
  // The concept of identity does NOT involve feedback-looping timeout fudge
  // Nor is the acceptable size of a response necessary to distinguish.
  return url == r.url && accept == r.accept;
}
auto Self::operator<(HttpRequestDescription const& r) const -> bool {
  return url == r.url ? accept < r.accept : url < r.url;
}