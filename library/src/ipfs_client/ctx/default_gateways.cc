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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 12'009},
      {"https://jcsl.hopto.org/", 337},
      {"https://ipfs.io/", 299},
      {"https://gateway.ipfs.io/", 285},
      {"https://ipfs.eth.aragon.network/", 272},
      {"https://cloudflare-ipfs.com/", 168},
      {"https://ipfs.joaoleitao.org/", 33},
      {"https://delegated-ipfs.dev/", 9},
      {"https://human.mypinata.cloud/", 7},
      {"https://hardbin.com/", 7},
      {"https://dweb.link/", 6},
      {"https://ipfs.fleek.co/", 6},
      {"https://w3s.link/", 6},
      {"https://cf-ipfs.com/", 6},
      {"https://trustless-gateway.link/", 5},
      {"https://data.filstorage.io/", 5},
      {"https://ipfs.runfission.com/", 4},
      {"https://f010479.twinquasar.io/", 4},
      {"https://gateway.pinata.cloud/", 4},
      {"https://ipfs.omnicloudstorage.com:9443/", 4},
      {"https://ipfs.greyh.at/", 3},
      {"https://storry.tv/", 3},
      {"https://permaweb.eu.org/", 2},
      {"https://4everland.io/", 2},
      {"https://nftstorage.link/", 2},
      {"https://cesginc.com/", 0}};
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
