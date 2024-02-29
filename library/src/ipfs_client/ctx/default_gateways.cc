#include <ipfs_client/ctx/default_gateways.h>

#include <ipfs_client/ctx/gateway_config.h>

#include <sstream>

namespace ctx = ipfs::ctx;

bool ctx::LoadGatewaysFromEnvironmentVariable(ipfs::ctx::GatewayConfig& cfg) {
  auto* ovr = std::getenv("IPFS_GATEWAY");
  if (!ovr) {
    return false;
  }
  std::istringstream user_override{ovr};
  std::string gw;
  bool at_least_one = false;
  while (user_override >> gw) {
    if (gw.empty()) {
      continue;
    }
    if (gw.back() != '/') {
      gw.push_back('/');
    }
    cfg.AddGateway(gw, 300U);
    at_least_one = true;
  }
  return at_least_one;
}

void ctx::LoadStaticGatewayList(ipfs::ctx::GatewayConfig& cfg) {
  auto static_list = {
      std::pair<std::string_view, int>{"http://localhost:8080/", 12'086},
      {"https://ipfs.io/", 743},
      {"https://jcsl.hopto.org/", 692},
      {"https://gateway.ipfs.io/", 559},
      {"https://human.mypinata.cloud/", 283},
      {"https://dag.w3s.link/", 158},
      {"https://ipfs.runfission.com/", 81},
      {"https://permaweb.eu.org/", 48},
      {"https://delegated-ipfs.dev/", 38},
      {"https://ipfs.joaoleitao.org/", 30},
      {"https://dweb.link/", 10},
      {"https://cesginc.com/", 9},
      {"https://f010479.twinquasar.io/", 8},
      {"https://http.f02620.devtty.eu/", 7},
      {"https://gateway.pinata.cloud/", 6},
      {"https://ipfs.fleek.co/", 5},
      {"https://nftstorage.link/", 4},
      {"https://data.filstorage.io/", 3},
      {"https://hardbin.com/", 2},
      {"https://ipfs.omnicloudstorage.com:9443/", 1},
      {"https://w3s.link/", 0},
      {"https://ipfs.jpu.jp/", 0}};
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
