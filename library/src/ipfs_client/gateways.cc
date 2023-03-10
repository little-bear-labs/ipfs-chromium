#include "ipfs_client/gateways.h"
#include "vocab/log_macros.h"

#include <algorithm>
#include <string>

using namespace std::string_literals;

ipfs::Gateways::Gateways()
    : known_gateways_{{"http://localhost:8080/"s, 86},
                      {"https://ipfs.io/"s, 67},
                      {"https://gateway.ipfs.io/"s, 43},
                      {"https://jcsl.hopto.org/"s, 23},
                      {"https://dweb.link/"s, 22},
                      {"https://ipfs.joaoleitao.org/"s, 19},
                      {"https://ipfs.best-practice.se/"s, 18},
                      {"https://92.119.16.150/"s, 17},
                      {"https://192.158.233.119/"s, 16},
                      {"https://191.96.101.86/"s, 15},
                      {"https://38.92.47.56/"s, 14},
                      {"https://192.158.233.115/"s, 13},
                      {"https://ipfs.jpu.jp/"s, 12},
                      {"https://64.44.166.184/"s, 11},
                      {"https://jorropo.net/"s, 11},
                      {"https://192.158.233.122/"s, 11},
                      {"https://192.158.233.118/"s, 11},
                      {"https://192.158.233.117/"s, 10},
                      {"https://192.158.233.116/"s, 9},
                      {"https://storry.tv/"s, 8},
                      {"https://video.oneloveipfs.com/"s, 7},
                      {"https://ipfs.runfission.com/"s, 5},
                      {"https://ipfs.storry.tv/"s, 4},
                      {"https://gateway.pinata.cloud/"s, 3},
                      {"https://ipfs-gateway.cloud/"s, 1},
                      {"https://ipfs.litnet.work/"s, 0}

      },
      random_engine_{std::random_device{}()},
      dist_{0.0625} {}
ipfs::Gateways::~Gateways() {}

auto ipfs::Gateways::GenerateList() const -> GatewayList {
  GatewayList result;
  for (auto [k, v] : known_gateways_) {
    //    result.push_back(std::make_shared<Gateway>(k, v +
    //    dist_(random_engine_)));
    result.push_back({k, v + dist_(random_engine_)});
  }
  std::sort(result.begin(), result.end());
  //,            [](auto& a, auto& b) { return *a < *b; }
  //  );
  return result;
}

void ipfs::Gateways::promote(std::string const& key) {
  LOG(INFO) << "promote(" << key << ")";
  known_gateways_.at(key)++;
}
void ipfs::Gateways::demote(std::string const& key) {
  LOG(INFO) << "demote(" << key << ")";
  auto prio = known_gateways_.at(key);
  if (prio > 1) {
    --prio;
  }
}
