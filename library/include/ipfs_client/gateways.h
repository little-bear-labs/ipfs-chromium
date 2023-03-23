#ifndef CHROMIUM_IPFS_GATEWAYS_H_
#define CHROMIUM_IPFS_GATEWAYS_H_

#include "export.h"
#include "gateway.h"
#include "vocab/flat_mapset.h"

#include <ctime>
#include <memory>
#include <random>
#include <string>
#include <string_view>
#include <vector>

namespace ipfs {
using GatewayList = std::vector<Gateway>;
class NetworkingApi;

class Gateways {
  flat_map<std::string, unsigned> known_gateways_;
  std::default_random_engine random_engine_;
  std::geometric_distribution<unsigned> dist_;

 public:
  Gateways();
  ~Gateways();
  GatewayList GenerateList();
  void promote(std::string const&);
  void demote(std::string const&);
  void AddGateways(std::vector<std::string>);
};
}  // namespace ipfs

#endif  // CHROMIUM_IPFS_GATEWAYS_H_
