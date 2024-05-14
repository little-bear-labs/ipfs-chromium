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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 12'006},
      {"https://jcsl.hopto.org/", 325},
      {"https://ipfs.eth.aragon.network/", 275},
      {"https://gateway.ipfs.io/", 267},
      {"https://ipfs.io/", 261},
      {"https://cloudflare-ipfs.com/", 167},
      {"https://cf-ipfs.com/", 9},
      {"https://ipfs.runfission.com/", 7},
      {"https://delegated-ipfs.dev/", 6},
      {"https://ipfs.joaoleitao.org/", 6},
      {"https://trustless-gateway.link/", 6},
      {"https://human.mypinata.cloud/", 5},
      {"https://ipfs.fleek.co/", 5},
      {"https://permaweb.eu.org/", 5},
      {"https://f010479.twinquasar.io/", 4},
      {"https://gateway.pinata.cloud/", 3},
      {"https://dweb.link/", 3},
      {"https://w3s.link/", 3},
      {"https://data.filstorage.io/", 3},
      {"https://hardbin.com/", 2},
      {"https://ipfs.omnicloudstorage.com:9443/", 2},
      {"https://4everland.io/", 2},
      {"https://nftstorage.link/", 1},
      {"https://storry.tv/", 1},
      {"https://ipfs.greyh.at/", 0}};
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
