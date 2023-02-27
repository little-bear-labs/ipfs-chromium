#include "ipfs_client/gateways.h"
#include "vocab/log_macros.h"

#include <algorithm>
#include <string>

using namespace std::string_literals;

ipfs::Gateways::Gateways()
    : known_gateways_{{"http://localhost:8080/"s, 91},
                      {"https://jcsl.hopto.org/"s, 15},
                      {"https://dweb.link/"s, 11},
                      {"https://gateway.ipfs.io/"s, 11},
                      {"https://gateway.pinata.cloud/"s, 6},
                      {"https://ipfs.best-practice.se/"s, 10},
                      {"https://ipfs-gateway.cloud/"s, 1},
                      {"https://ipfs.io/"s, 11},
                      {"https://ipfs.joaoleitao.org/"s, 12},
                      {"https://ipfs.storry.tv/"s, 11},
                      {"https://jorropo.net/"s, 11},
                      {"https://ipfs.jpu.jp/"s, 11},
                      {"https://ipfs.litnet.work/"s, 5},
                      {"https://ipfs.runfission.com/"s, 8},
                      {"https://jorropo.net/"s, 10},
                      {"https://video.oneloveipfs.com/"s, 10},
                      {"https://storry.tv/"s, 11},
                      {"https://138.199.2.57/"s, 11},
                      {"https://92.119.16.147/"s, 10},
                      {"https://92.119.16.134/"s, 12},
                      {"https://92.119.16.150/"s, 12},
                      {"https://92.119.16.153/"s, 10},
                      {"https://192.158.233.116/"s, 12},
                      {"https://192.158.233.119/"s, 12},
                      {"https://192.158.233.122/"s, 11},
                      {"https://192.158.233.117/"s, 12},
                      {"https://192.158.233.121/"s, 12},
                      {"https://192.158.233.115/"s, 12},
                      {"https://192.158.233.118/"s, 12},
                      {"https://191.96.101.86/"s, 12},
                      {"https://191.96.101.87/"s, 11},
                      {"https://64.44.166.184/"s, 12},
                      {"https://38.92.47.56/"s, 12}

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
  L_INF("promote(" << key << ")");
  known_gateways_.at(key)++;
}
void ipfs::Gateways::demote(std::string const& key) {
  L_INF("demote(" << key << ")");
  auto prio = known_gateways_.at(key);
  if (prio > 1) {
    --prio;
  }
}
