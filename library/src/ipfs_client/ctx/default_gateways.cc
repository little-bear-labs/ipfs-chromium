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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 12'021},
      {"https://jcsl.hopto.org/", 380},
      {"https://ipfs.io/", 341},
      {"https://gateway.ipfs.io/", 325},
      {"https://ipfs.eth.aragon.network/", 275},
      {"https://cloudflare-ipfs.com/", 194},
      {"https://ipfs.joaoleitao.org/", 50},
      {"https://cf-ipfs.com/", 40},
      {"https://hardbin.com/", 6},
      {"https://trustless-gateway.link/", 6},
      {"https://human.mypinata.cloud/", 6},
      {"https://delegated-ipfs.dev/", 6},
      {"https://ipfs.greyh.at/", 5},
      {"https://nftstorage.link/", 4},
      {"https://dweb.link/", 4},
      {"https://dag.w3s.link/", 4},
      {"https://w3s.link/", 4},
      {"https://ipfs.fleek.co/", 4},
      {"https://permaweb.eu.org/", 3},
      {"https://ipfs.runfission.com/", 3},
      {"https://cesginc.com/", 3},
      {"https://4everland.io/", 3},
      {"https://ipfs.omnicloudstorage.com:9443/", 2},
      {"https://f010479.twinquasar.io/", 2},
      {"https://data.filstorage.io/", 2},
      {"https://gateway.pinata.cloud/", 1}};
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
