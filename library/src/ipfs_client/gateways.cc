#include <ipfs_client/gateways.h>

#include <ipfs_client/context_api.h>

#include "log_macros.h"

#include <algorithm>
#include <string>

using namespace std::string_literals;

ipfs::Gateways::Gateways()
    : random_engine_{std::random_device{}()}, dist_{0.01} {
  auto gws = DefaultGateways();
  for (auto [k, v] : gws) {
    known_gateways_[k] = v;
  }
}
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

std::vector<std::pair<std::string, int>> ipfs::Gateways::DefaultGateways() {
  return {{"http://localhost:8080/"s, 896},
          {"https://ipfs.io/"s, 894},
          {"https://gateway.ipfs.io/"s, 892},
          {"https://dweb.link/"s, 885},
          {"https://jcsl.hopto.org/"s, 873},
          {"https://ipfs.joaoleitao.org/"s, 816},
          {"https://jorropo.net/"s, 333},
          {"https://ipfs.jpu.jp/"s, 319},
          {"https://gateway.pinata.cloud/"s, 288},
          {"https://ipfs.runfission.com/"s, 218},
          {"https://nftstorage.link/"s, 175},
          {"https://w3s.link/"s, 142},
          {"https://ipfs.soul-network.com/"s, 72},
          {"https://ipfs.scalaproject.io/"s, 70},
          {"https://storry.tv/"s, 51},
          {"https://hardbin.com/"s, 47},
          {"https://ipfs.fleek.co/"s, 46},
          {"https://permaweb.eu.org/"s, 43},
          {"https://ipfs-gateway.cloud/"s, 31},
          {"https://ipfs.storry.tv/"s, 11},
          {"https://ipfs.anonymize.com/"s, 0}};
}
