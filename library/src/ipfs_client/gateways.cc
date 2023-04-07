#include "ipfs_client/gateways.h"
#include "ipfs_client/networking_api.h"
#include "log_macros.h"

#include <algorithm>
#include <string>

using namespace std::string_literals;

ipfs::Gateways::Gateways()
    : known_gateways_{{"http://localhost:8080/"s, 890},
                      {"https://ipfs.io/"s, 888},
                      {"https://gateway.ipfs.io/"s, 875},
                      {"https://dweb.link/"s, 816},
                      {"https://jcsl.hopto.org/"s, 559},
                      {"https://ipfs.joaoleitao.org/"s, 542},
                      {"https://ipfs.best-practice.se/"s, 461},
                      {"https://jorropo.net/"s, 338},
                      {"https://ipfs.jpu.jp/"s, 213},
                      {"https://gateway.pinata.cloud/"s, 163},
                      {"https://ipfs.runfission.com/"s, 99},
                      {"https://ipfs.soul-network.com/"s, 78},
                      {"https://nftstorage.link/"s, 77},
                      {"https://w3s.link/"s, 73},
                      {"https://ipfs.scalaproject.io/"s, 62},
                      {"https://storry.tv/"s, 57},
                      {"https://hardbin.com/"s, 52},
                      {"https://cf-ipfs.com/"s, 42},
                      {"https://ipfs-gateway.cloud/"s, 37},
                      {"https://ipfs.fleek.co/"s, 32},
                      {"https://permaweb.eu.org/"s, 28},
                      {"https://ipfs.storry.tv/"s, 17},
                      {"https://dweb.eu.org/"s, 12},
                      {"https://cloudflare-ipfs.com/"s, 8},
                      {"https://ipfs.anonymize.com/"s, 3}

      },
      random_engine_{std::random_device{}()},
      dist_{0.01} {}
ipfs::Gateways::~Gateways() {}

auto ipfs::Gateways::GenerateList() -> GatewayList {
  GatewayList result;
  for (auto [k, v] : known_gateways_) {
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
