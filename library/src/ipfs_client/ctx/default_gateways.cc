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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 11'900},
      {"https://gateway.ipfs.io/", 485},
      {"https://ipfs.eth.aragon.network/", 294},
      {"https://cloudflare-ipfs.com/", 263},
      {"https://ipfs.io/", 262},
      {"https://human.mypinata.cloud/", 200},
      {"https://cf-ipfs.com/", 140},
      {"https://jcsl.hopto.org/", 100},
      {"https://delegated-ipfs.dev/", 80},
      {"https://ipfs.fleek.co/", 71},
      {"https://trustless-gateway.link/", 68},
      {"https://dweb.link/", 60},
      {"https://data.filstorage.io/", 59},
      {"https://hardbin.com/", 54},
      {"https://dag.w3s.link/", 48},
      {"https://permaweb.eu.org/", 42},
      {"https://ipfs.joaoleitao.org/", 36},
      {"https://ipfs.runfission.com/", 30},
      {"https://ipfs.greyh.at/", 26},
      {"https://gateway.pinata.cloud/", 19},
      {"https://w3s.link/", 14},
      {"https://nftstorage.link/", 10},
      {"https://4everland.io/", 9}};
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
