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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 12'070},
      {"https://ipfs.io/", 649},
      {"https://jcsl.hopto.org/", 600},
      {"https://gateway.ipfs.io/", 499},
      {"https://delegated-ipfs.dev/", 308},
      {"https://human.mypinata.cloud/", 176},
      {"https://ipfs.joaoleitao.org/", 134},
      {"https://dag.w3s.link/", 50},
      {"https://ipfs.eth.aragon.network/", 19},
      {"https://cloudflare-ipfs.com/", 16},
      {"https://cf-ipfs.com/", 15},
      {"https://ipfs.runfission.com/", 14},
      {"https://nftstorage.link/", 13},
      {"https://permaweb.eu.org/", 12},
      {"https://ipfs.fleek.co/", 11},
      {"https://dweb.link/", 10},
      {"https://trustless-gateway.link/", 9},
      {"https://4everland.io/", 8},
      {"https://cesginc.com/", 7},
      {"https://f010479.twinquasar.io/", 6},
      {"https://hardbin.com/", 5},
      {"https://data.filstorage.io/", 4},
      {"https://w3s.link/", 3},
      {"https://ipfs.omnicloudstorage.com:9443/", 2},
      {"https://http.f02620.devtty.eu/", 1},
      {"https://gateway.pinata.cloud/", 0}};
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
