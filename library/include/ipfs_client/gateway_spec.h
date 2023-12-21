#ifndef IPFS_GATEWAY_SPEC_H_
#define IPFS_GATEWAY_SPEC_H_

#include <string>

namespace ipfs {
struct GatewaySpec {
  std::string prefix;
  unsigned rate;
  bool operator<(GatewaySpec const& r) const {
    if (rate == r.rate) {
      return prefix < r.prefix;
    }
    return rate > r.rate;
  }
};
}  // namespace ipfs

#endif  // IPFS_GATEWAY_SPEC_H_
