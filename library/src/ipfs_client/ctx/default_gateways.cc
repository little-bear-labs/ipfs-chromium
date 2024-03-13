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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 12'050},
      {"https://ipfs.io/", 504},
      {"https://jcsl.hopto.org/", 491},
      {"https://gateway.ipfs.io/", 428},
      {"https://ipfs.eth.aragon.network/", 299},
      {"https://cloudflare-ipfs.com/", 264},
      {"https://cf-ipfs.com/", 116},
      {"https://ipfs.joaoleitao.org/", 77},
      {"https://ipfs.fleek.co/", 62},
      {"https://trustless-gateway.link/", 30},
      {"https://delegated-ipfs.dev/", 26},
      {"https://dweb.link/", 14},
      {"https://dag.w3s.link/", 13},
      {"https://hardbin.com/", 12},
      {"https://permaweb.eu.org/", 11},
      {"https://nftstorage.link/", 10},
      {"https://4everland.io/", 9},
      {"https://gateway.pinata.cloud/", 8},
      {"https://w3s.link/", 7},
      {"https://data.filstorage.io/", 6},
      {"https://cesginc.com/", 5},
      {"https://ipfs.runfission.com/", 4},
      {"https://human.mypinata.cloud/", 3},
      {"https://http.f02620.devtty.eu/", 2},
      {"https://ipfs.omnicloudstorage.com:9443/", 1},
      {"https://f010479.twinquasar.io/", 0}};
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
