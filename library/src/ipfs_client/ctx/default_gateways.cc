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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 11'997},
      {"https://jcsl.hopto.org/", 305},
      {"https://ipfs.eth.aragon.network/", 280},
      {"https://gateway.ipfs.io/", 260},
      {"https://ipfs.io/", 248},
      {"https://cloudflare-ipfs.com/", 162},
      {"https://cf-ipfs.com/", 13},
      {"https://human.mypinata.cloud/", 10},
      {"https://delegated-ipfs.dev/", 9},
      {"https://ipfs.fleek.co/", 8},
      {"https://dweb.link/", 8},
      {"https://trustless-gateway.link/", 7},
      {"https://data.filstorage.io/", 7},
      {"https://hardbin.com/", 7},
      {"https://permaweb.eu.org/", 6},
      {"https://ipfs.joaoleitao.org/", 6},
      {"https://ipfs.runfission.com/", 6},
      {"https://ipfs.greyh.at/", 6},
      {"https://4everland.io/", 5},
      {"https://dag.w3s.link/", 5},
      {"https://gateway.pinata.cloud/", 4},
      {"https://w3s.link/", 4},
      {"https://nftstorage.link/", 3}};
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
