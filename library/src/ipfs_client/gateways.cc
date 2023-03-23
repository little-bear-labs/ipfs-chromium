#include "ipfs_client/gateways.h"
#include "ipfs_client/networking_api.h"
#include "log_macros.h"

#include <algorithm>
#include <string>

using namespace std::string_literals;

ipfs::Gateways::Gateways()
    : known_gateways_{{"http://localhost:8080/"s, 889},
                      {"https://ipfs.io/"s, 869},
                      {"https://gateway.ipfs.io/"s, 819},
                      {"https://dweb.link/"s, 769},
                      {"https://jcsl.hopto.org/"s, 559},
                      {"https://ipfs.joaoleitao.org/"s, 489},
                      {"https://ipfs.best-practice.se/"s, 399},
                      {"https://jorropo.net/"s, 279},
                      {"https://ipfs.jpu.jp/"s, 149},
                      {"https://gateway.pinata.cloud/"s, 109},
                      {"https://ipfs.runfission.com/"s, 89},
                      {"https://storry.tv/"s, 59},
                      {"https://ipfs-gateway.cloud/"s, 39},
                      {"https://ipfs.storry.tv/"s, 19},
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
    LOG(WARNING) << "Can't demote " << key << " as I don't have that gateway.";
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
    if (known_gateways_.insert({prefix, 99}).second) {
      LOG(INFO) << "Adding discovered gateway " << prefix;
    }
  }
}
