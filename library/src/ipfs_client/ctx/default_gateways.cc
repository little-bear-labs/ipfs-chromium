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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 12'033},
      {"https://jcsl.hopto.org/", 419},
      {"https://ipfs.io/", 391},
      {"https://gateway.ipfs.io/", 368},
      {"https://ipfs.eth.aragon.network/", 281},
      {"https://cloudflare-ipfs.com/", 194},
      {"https://cf-ipfs.com/", 82},
      {"https://ipfs.greyh.at/", 77},
      {"https://trustless-gateway.link/", 50},
      {"https://ipfs.joaoleitao.org/", 44},
      {"https://permaweb.eu.org/", 7},
      {"https://hardbin.com/", 6},
      {"https://human.mypinata.cloud/", 6},
      {"https://dweb.link/", 6},
      {"https://ipfs.fleek.co/", 5},
      {"https://4everland.io/", 5},
      {"https://ipfs.runfission.com/", 5},
      {"https://w3s.link/", 4},
      {"https://nftstorage.link/", 3},
      {"https://ipfs.omnicloudstorage.com:9443/", 3},
      {"https://http.f02620.devtty.eu/", 3},
      {"https://data.filstorage.io/", 2},
      {"https://cesginc.com/", 2},
      {"https://delegated-ipfs.dev/", 2},
      {"https://f010479.twinquasar.io/", 1},
      {"https://dag.w3s.link/", 1},
      {"https://gateway.pinata.cloud/", 0}};
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
