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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 1018},
      {"https://ipfs.io/", 994},
      {"https://dweb.link/", 967},
      {"https://hardbin.com/", 951},
      {"https://ipfs.joaoleitao.org/", 887},
      {"https://ipfs.greyh.at/", 883},
      {"https://trustless-gateway.link/", 880},
      {"https://dlunar.net/", 729},
      {"https://flk-ipfs.io/", 713},
      {"https://ipfs.cyou/", 497},
      {"https://human.mypinata.cloud/", 436},
      {"https://jcsl.hopto.org/", 356},
      {"https://delegated-ipfs.dev/", 345},
      {"https://4everland.io/", 321},
      {"https://ipfs.runfission.com/", 291},
      {"https://gateway.pinata.cloud/", 165},
      {"https://dag.w3s.link/", 159},
      {"https://ipfs.eth.aragon.network/", 11},
      {"https://data.filstorage.io/", 10},
      {"https://storry.tv/", 9},

      //Currently redirects to https://ipfs.io
      {"https://cloudflare-ipfs.com/", 8},
      {"https://cf-ipfs.com/", 7},
      {"https://fleek.ipfs.io/", 6},
      {"https://ipfs.fleek.co/", 5},
      {"https://permaweb.eu.org/", 4},
      {"https://gateway.ipfs.io/", 3},

      //Currently redirects to https://dweb.link/
      {"https://nftstorage.link/", 2},
      {"https://w3s.link/", 1}
    };
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
