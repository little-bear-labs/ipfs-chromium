#ifndef IPFS_GATEWAY_SPEC_H_
#define IPFS_GATEWAY_SPEC_H_

#include <ipfs_client/gw/gateway_request_type.h>
#include <array>
#include <string>

namespace ipfs {
struct GatewaySpec {
  std::string prefix;
  unsigned rate;
  std::array<int, gw::RequestTypeCount> request_type_affinity;

  bool operator<(GatewaySpec const& r) const {
    if (rate == r.rate) {
      return prefix < r.prefix;
    }
    return rate > r.rate;
  }
  explicit GatewaySpec(std::string_view pre, unsigned r)
      : prefix{pre}, rate{r} {
    request_type_affinity.fill(0);
  }
};
}  // namespace ipfs

#endif  // IPFS_GATEWAY_SPEC_H_
