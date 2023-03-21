#include "ipfs_client/gateways.h"
#include "log_macros.h"

#include <algorithm>
#include <string>

using namespace std::string_literals;

ipfs::Gateways::Gateways()
    : known_gateways_{{"http://localhost:8080/"s, 84},
                      {"https://ipfs.io/"s, 69},
                      {"https://gateway.ipfs.io/"s, 56},
                      {"https://dweb.link/"s, 34},
                      {"https://jcsl.hopto.org/"s, 28},
                      {"https://ipfs.joaoleitao.org/"s, 21},
                      {"https://ipfs.best-practice.se/"s, 19},
                      {"https://92.119.16.150/"s, 18},
                      {"https://192.158.233.119/"s, 16},
                      {"https://191.96.101.86/"s, 15},
                      {"https://38.92.47.56/"s, 14},
                      {"https://192.158.233.115/"s, 13},
                      {"https://jorropo.net/"s, 12},
                      {"https://ipfs.jpu.jp/"s, 12},
                      {"https://192.158.233.122/"s, 11},
                      {"https://192.158.233.118/"s, 11},
                      {"https://192.158.233.117/"s, 10},
                      {"https://storry.tv/"s, 7},
                      {"https://video.oneloveipfs.com/"s, 7},
                      {"https://gateway.pinata.cloud/"s, 4},
                      {"https://ipfs.runfission.com/"s, 4},
                      {"https://ipfs.storry.tv/"s, 2},
                      {"https://ipfs-gateway.cloud/"s, 1},
                      {"https://ipfs.litnet.work/"s, 0}

      },
      random_engine_{std::random_device{}()},
      dist_{0.0625} {}
ipfs::Gateways::~Gateways() {}

auto ipfs::Gateways::GenerateList() const -> GatewayList {
  GatewayList result;
  for (auto [k, v] : known_gateways_) {
    result.push_back({k, v + dist_(random_engine_)});
    result.push_back({k, v + dist_(random_engine_)});
    result.push_back({k, v + dist_(random_engine_)});
  }
  std::sort(result.begin(), result.end());
  return result;
}

void ipfs::Gateways::promote(std::string const& key) {
  auto it = known_gateways_.find(key);
  if (known_gateways_.end() == it) {
    LOG(ERROR) << "Can't promote (" << key
               << ") because I don't know that one.";
  } else {
    LOG(INFO) << "promote(" << key << ")";
    known_gateways_.at(key)++;
  }
}
void ipfs::Gateways::demote(std::string const& key) {
  LOG(INFO) << "demote(" << key << ")";
  auto prio = known_gateways_.at(key);
  if (prio > 1) {
    --prio;
  }
}
