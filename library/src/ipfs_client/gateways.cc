#include <ipfs_client/gateways.h>

#include <ipfs_client/context_api.h>

#include "log_macros.h"

#include <algorithm>
#include <string>
#include <sstream>

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
    auto l = known_gateways_.at(key)++;
    if (l % (++up_log_ / 2) <= 9) {
      LOG(INFO) << "Promote(" << key << ")";
    }
  }
}
void ipfs::Gateways::demote(std::string const& key) {
  auto it = known_gateways_.find(key);
  if (known_gateways_.end() == it) {
    VLOG(2) << "Can't demote " << key << " as I don't have that gateway.";
  } else if (it->second) {
    if (it->second-- % 3 == 0) {
      LOG(INFO) << "Demote(" << key << ") to " << it->second;
    }
  } else {
    LOG(INFO) << "Demoted(" << key << ") for the last time - dropping.";
    known_gateways_.erase(it);
  }
}

void ipfs::Gateways::AddGateways(std::vector<std::string> v) {
  LOG(INFO) << "AddGateways(" << v.size() << ')';
  for (auto& ip : v) {
    if (ip.empty()) {
      LOG(ERROR) << "ERROR: Attempted to add empty string as gateway!";
      continue;
    }
    std::string prefix;
    if (ip.find("://") == std::string::npos) {
      prefix = "http://";
      prefix.append(ip);
    } else {
      prefix = ip;
    }
    if (prefix.back() != '/') {
      prefix.push_back('/');
    }
    if (known_gateways_.insert({prefix, 99}).second) {
      VLOG(1) << "Adding discovered gateway " << prefix;
    }
  }
}

auto ipfs::Gateways::DefaultGateways() -> GatewayList {
  auto* ovr = std::getenv("IPFS_GATEWAY");
  if (ovr && *ovr) {
    std::istringstream user_override{ovr};
    GatewayList result;
    std::string gw;
    while (user_override >> gw) {
      if ( gw.empty() ) {
        continue;
      }
      if ( gw.back() != '/' ) {
        gw.push_back('/');
      }
      result.push_back( {gw, 0} );
    }
    auto N = static_cast<int>(result.size());
    for (auto i = 0; i < N; ++i) {
      auto& r = result[i];
      r.strength = N - i;
      LOG(INFO) << "User-specified gateway: " << r.prefix << '=' << r.strength;
    }
    return result;
  }
  return {{"http://localhost:8080/"s, 932},
          {"https://jcsl.hopto.org/"s, 920},
          {"https://human.mypinata.cloud/"s, 883},
          {"https://ipfs.io/"s, 847},
          {"https://gateway.ipfs.io/"s, 741},
          {"https://dweb.link/"s, 619},
          {"https://gateway.pinata.cloud/"s, 497},
          {"https://ipfs.joaoleitao.org/"s, 438},
          {"https://ipfs.runfission.com/"s, 376},
          {"https://nftstorage.link/"s, 311},
          {"https://w3s.link/"s, 246},
          {"https://ipfs.fleek.co/"s, 207},
          {"https://ipfs.jpu.jp/"s, 173},
          {"https://permaweb.eu.org/"s, 126},
          {"https://jorropo.net/"s, 79},
          {"https://hardbin.com/"s, 41},
          {"https://ipfs.soul-network.com/"s, 1},
          {"https://storry.tv/"s, 0}};
}
