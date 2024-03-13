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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 12'060},
      {"https://ipfs.io/", 589},
      {"https://jcsl.hopto.org/", 539},
      {"https://gateway.ipfs.io/", 480},
      {"https://delegated-ipfs.dev/", 142},
      {"https://ipfs.joaoleitao.org/", 141},
      {"https://human.mypinata.cloud/", 51},
      {"https://ipfs.eth.aragon.network/", 26},
      {"https://cloudflare-ipfs.com/", 23},
      {"https://cf-ipfs.com/", 22},
      {"https://ipfs.fleek.co/", 17},
      {"https://dag.w3s.link/", 14},
      {"https://trustless-gateway.link/", 13},
      {"https://ipfs.runfission.com/", 12},
      {"https://permaweb.eu.org/", 11},
      {"https://dweb.link/", 10},
      {"https://hardbin.com/", 9},
      {"https://4everland.io/", 8},
      {"https://cesginc.com/", 7},
      {"https://data.filstorage.io/", 6},
      {"https://w3s.link/", 5},
      {"https://gateway.pinata.cloud/", 4},
      {"https://http.f02620.devtty.eu/", 3},
      {"https://nftstorage.link/", 2},
      {"https://ipfs.omnicloudstorage.com:9443/", 1},
      {"https://f010479.twinquasar.io/", 0}};
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
