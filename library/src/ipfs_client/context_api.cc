#include <ipfs_client/context_api.h>

#include <ipfs_client/gateway.h>

bool ipfs::HttpRequestDescription::operator==(
    HttpRequestDescription const& r) const {
  return url == r.url && accept == r.accept;
}
bool ipfs::HttpRequestDescription::operator<(
    HttpRequestDescription const& r) const {
  return url == r.url ? accept < r.accept : url < r.url;
}