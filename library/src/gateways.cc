#include "ipfs_client/gateways.h"

#include <string>

using namespace std::string_literals;

ipfs::Gateways::Gateways()
    : known_gateways_{{"http://localhost:8080/"s, 99},
                      {"https://4everland.io/"s, 10},
                      {"https://c4rex.co/"s, 10},
                      {"https://cloudflare-ipfs.com/"s, 10},
                      {"https://dweb.link/"s, 11},
                      {"https://gateway.ipfs.io/"s, 11},
                      {"https://gateway.pinata.cloud/"s, 10},
                      {"https://ipfs.czip.it/"s, 11},
                      {"https://ipfs.best-practice.se/"s, 11},
                      {"https://ipfs-gateway.cloud/"s, 11},
                      {"https://ipfs.io/"s, 11},
                      {"https://ipfs.joaoleitao.org/"s, 12},
                      {"https://ipfs.storry.tv/"s, 11},
                      {"https://jorropo.net/"s, 11},
                      {"https://ipfs.jpu.jp/"s, 11},
                      {"https://ipfs.litnet.work/"s, 11},
                      {"https://ipfs.runfission.com/"s, 10},
                      {"https://jorropo.net/"s, 10},
                      {"https://hardbin.com/"s, 10},
                      {"https://nftstorage.link/"s, 10},
                      {"https://video.oneloveipfs.com/"s, 10},
                      {"https://cf-ipfs.com/"s, 10},
                      {"https://storry.tv/"s, 11},
                      {"https://via0.com/"s, 10},
                      {"https://w3s.link/"s, 10},
                      {"https://138.199.2.57/"s, 88},
                      {"https://172.93.54.119/"s, 88},
                      {"https://92.119.16.163/"s, 88},
                      {"https://172.93.54.59/"s, 88},
                      {"https://92.119.16.151/"s, 88},
                      {"https://92.119.16.147/"s, 88},
                      {"https://92.119.16.134/"s, 88},
                      {"https://92.119.16.150/"s, 88},
                      {"https://92.119.16.153/"s, 88},
                      {"https://67.21.32.23/"s, 88},
                      {"https://192.158.233.116/"s, 88},
                      {"https://192.158.233.120/"s, 88},
                      {"https://192.158.233.119/"s, 88},
                      {"https://192.158.233.122/"s, 88},
                      {"https://192.158.233.117/"s, 88},
                      {"https://192.158.233.114/"s, 88},
                      {"https://192.158.233.121/"s, 88},
                      {"https://192.158.233.115/"s, 88},
                      {"https://192.158.233.118/"s, 88},
                      {"https://191.96.101.86/"s, 88},
                      {"https://191.96.101.87/"s, 88},
                      {"https://64.44.166.184/"s, 88},
                      {"https://38.92.47.51/"s, 88},
                      {"https://38.92.47.56/"s, 88}

      },
      random_engine_{std::random_device{}()},
      dist_{0.0625} {}
ipfs::Gateways::~Gateways() {}

auto ipfs::Gateways::GenerateList() const -> GatewayList {
  GatewayList result;
  for (auto [k, v] : known_gateways_) {
    result.emplace(k, v + dist_(random_engine_));
  }
  return result;
}

void ipfs::Gateways::promote(std::string const& key) {
  known_gateways_.at(key)++;
}
void ipfs::Gateways::demote(std::string const& key) {
  auto prio = known_gateways_.at(key);
  if (prio > 1) {
    --prio;
  }
}
