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
class ContextApi;

class Gateways {
  flat_map<std::string, int> known_gateways_;
  std::default_random_engine random_engine_;
  std::geometric_distribution<unsigned> dist_;
  int up_log_ = 1;

 public:
  static std::vector<std::pair<std::string, int>> DefaultGateways();

  Gateways();
  ~Gateways();
  GatewayList GenerateList();
  void promote(std::string const&);
  void demote(std::string const&);
  void AddGateways(std::vector<std::string>);
};
}  // namespace ipfs

#endif  // CHROMIUM_IPFS_GATEWAYS_H_
