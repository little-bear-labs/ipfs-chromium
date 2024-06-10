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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 11'996},
      {"https://jcsl.hopto.org/", 304},
      {"https://ipfs.eth.aragon.network/", 279},
      {"https://gateway.ipfs.io/", 259},
      {"https://ipfs.io/", 247},
      {"https://cloudflare-ipfs.com/", 161},
      {"https://cf-ipfs.com/", 12},
      {"https://human.mypinata.cloud/", 8},
      {"https://delegated-ipfs.dev/", 7},
      {"https://ipfs.fleek.co/", 7},
      {"https://dweb.link/", 7},
      {"https://trustless-gateway.link/", 6},
      {"https://data.filstorage.io/", 6},
      {"https://hardbin.com/", 6},
      {"https://permaweb.eu.org/", 5},
      {"https://ipfs.joaoleitao.org/", 5},
      {"https://ipfs.runfission.com/", 5},
      {"https://ipfs.greyh.at/", 5},
      {"https://4everland.io/", 4},
      {"https://dag.w3s.link/", 4},
      {"https://gateway.pinata.cloud/", 3},
      {"https://w3s.link/", 3},
      {"https://nftstorage.link/", 2}};
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
