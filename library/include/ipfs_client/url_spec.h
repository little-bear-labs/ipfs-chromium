#ifndef IPFS_URL_SPEC_H_
#define IPFS_URL_SPEC_H_

// TODO - Give more thought to how this interplays with gw::Request

#include <string>
#include <string_view>

namespace ipfs {
struct UrlSpec {
  std::string suffix;
  std::string_view accept;

  bool operator<(UrlSpec const& rhs) const {
    if (suffix != rhs.suffix) {
      return suffix < rhs.suffix;
    }
    return accept < rhs.accept;
  }
  bool none() const { return suffix.empty(); }
};
}  // namespace ipfs

#endif  // IPFS_URL_SPEC_H_
