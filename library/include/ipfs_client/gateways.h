#ifndef CHROMIUM_IPFS_GATEWAYS_H_
#define CHROMIUM_IPFS_GATEWAYS_H_

#include "export.h"
#include "gateway.h"
#include "vocab/flat_mapset.h"

#include <random>
#include <string>
#include <string_view>
#include <vector>

namespace ipfs {
using GatewayList = flat_set<Gateway>;

class Gateways {
  flat_map<std::string, unsigned> known_gateways_;
  // TODO - no way mutable is allowed to slide
  mutable std::default_random_engine random_engine_;
  mutable std::geometric_distribution<unsigned> dist_;

 public:
  Gateways();
  ~Gateways();
  GatewayList GenerateList() const;
  void promote(std::string const&);
  void demote(std::string const&);
};
}  // namespace ipfs

#endif  // CHROMIUM_IPFS_GATEWAYS_H_
