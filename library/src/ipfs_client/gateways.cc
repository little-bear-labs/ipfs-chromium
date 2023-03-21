#include "ipfs_client/gateways.h"
#include "ipfs_client/networking_api.h"
#include "log_macros.h"

#include <algorithm>
#include <string>

using namespace std::string_literals;

ipfs::Gateways::Gateways()
    : known_gateways_{{"http://localhost:8080/"s, 86},
                      {"https://ipfs.io/"s, 81},
                      {"https://gateway.ipfs.io/"s, 72},
                      {"https://dweb.link/"s, 62},
                      {"https://jcsl.hopto.org/"s, 41},
                      {"https://ipfs.joaoleitao.org/"s, 33},
                      {"https://ipfs.best-practice.se/"s, 27},
                      {"https://191.96.101.86/"s, 15},
                      {"https://ipfs.jpu.jp/"s, 14},
                      {"https://38.92.47.56/"s, 14},
                      {"https://jorropo.net/"s, 12},
                      {"https://192.158.233.122/"s, 11},
                      {"https://video.oneloveipfs.com/"s, 8},
                      {"https://storry.tv/"s, 7},
                      {"https://gateway.pinata.cloud/"s, 5},
                      {"https://ipfs.runfission.com/"s, 4},
                      {"https://ipfs.storry.tv/"s, 2},
                      {"https://ipfs-gateway.cloud/"s, 1},
                      {"https://ipfs.litnet.work/"s, 0}

      },
      random_engine_{std::random_device{}()},
      dist_{0.01} {}
ipfs::Gateways::~Gateways() {}

auto ipfs::Gateways::GenerateList(NetworkingApi* api) -> GatewayList {
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
  auto it = known_gateways_.find(key);
  if (known_gateways_.end() == it) {
    LOG(ERROR) << "Can't demote " << key << " as I don't have that gateway.";
  } else if (it->second) {
    it->second--;
    LOG(INFO) << "Demote(" << key << ") to " << it->second;
  } else {
    LOG(INFO) << "Demoted(" << key << ") for the last time - dropping.";
    known_gateways_.erase(it);
  }
}

void ipfs::Gateways::AddGateways(std::vector<std::string> v) {
  LOG(INFO) << "AddGateways(" << v.size() << ')';
  for (auto& ip : v) {
    std::string prefix{"http://"};
    prefix.append(ip);
    prefix.push_back('/');
    if (known_gateways_.insert({prefix, 9}).second) {
      LOG(INFO) << "Adding discovered gateway " << prefix;
    }
  }
}
