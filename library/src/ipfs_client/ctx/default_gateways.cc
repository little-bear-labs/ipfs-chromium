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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 12'030},
      {"https://jcsl.hopto.org/", 395},
      {"https://ipfs.io/", 364},
      {"https://gateway.ipfs.io/", 345},
      {"https://ipfs.eth.aragon.network/", 272},
      {"https://cloudflare-ipfs.com/", 191},
      {"https://cf-ipfs.com/", 60},
      {"https://ipfs.joaoleitao.org/", 47},
      {"https://trustless-gateway.link/", 12},
      {"https://ipfs.greyh.at/", 11},
      {"https://hardbin.com/", 7},
      {"https://human.mypinata.cloud/", 5},
      {"https://4everland.io/", 5},
      {"https://dweb.link/", 5},
      {"https://permaweb.eu.org/", 5},
      {"https://delegated-ipfs.dev/", 4},
      {"https://nftstorage.link/", 4},
      {"https://ipfs.fleek.co/", 3},
      {"https://ipfs.omnicloudstorage.com:9443/", 3},
      {"https://cesginc.com/", 3},
      {"https://f010479.twinquasar.io/", 3},
      {"https://data.filstorage.io/", 2},
      {"https://dag.w3s.link/", 2},
      {"https://w3s.link/", 2},
      {"https://ipfs.runfission.com/", 1},
      {"https://gateway.pinata.cloud/", 1},
      {"https://http.f02620.devtty.eu/", 0}};
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
