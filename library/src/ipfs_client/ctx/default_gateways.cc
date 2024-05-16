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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 12'005},
      {"https://jcsl.hopto.org/", 319},
      {"https://ipfs.eth.aragon.network/", 276},
      {"https://gateway.ipfs.io/", 260},
      {"https://ipfs.io/", 248},
      {"https://cloudflare-ipfs.com/", 162},
      {"https://cf-ipfs.com/", 10},
      {"https://trustless-gateway.link/", 7},
      {"https://permaweb.eu.org/", 7},
      {"https://human.mypinata.cloud/", 5},
      {"https://ipfs.runfission.com/", 4},
      {"https://ipfs.joaoleitao.org/", 4},
      {"https://f010479.twinquasar.io/", 4},
      {"https://gateway.pinata.cloud/", 4},
      {"https://dweb.link/", 4},
      {"https://w3s.link/", 4},
      {"https://data.filstorage.io/", 4},
      {"https://delegated-ipfs.dev/", 3},
      {"https://hardbin.com/", 3},
      {"https://4everland.io/", 3},
      {"https://ipfs.fleek.co/", 2},
      {"https://ipfs.omnicloudstorage.com:9443/", 2},
      {"https://nftstorage.link/", 2},
      {"https://ipfs.greyh.at/", 1},
      {"https://storry.tv/", 0}};
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
