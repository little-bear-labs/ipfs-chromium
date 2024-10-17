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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 1029},
      {"https://ipfs.io/", 1008},
      {"https://dweb.link/", 955},
      {"https://trustless-gateway.link/", 945},
      {"https://hardbin.com/", 935},
      {"https://ipfs.joaoleitao.org/", 873},
      {"https://ipfs.greyh.at/", 872},
      {"https://dlunar.net/", 713},
      {"https://flk-ipfs.io/", 697},
      {"https://ipfs.cyou/", 483},
      {"https://human.mypinata.cloud/", 424},
      {"https://jcsl.hopto.org/", 386},
      {"https://delegated-ipfs.dev/", 336},
      {"https://4everland.io/", 310},
      {"https://ipfs.runfission.com/", 280},
      {"https://gateway.pinata.cloud/", 153},
      {"https://dag.w3s.link/", 147},
      {"https://ipfs.eth.aragon.network/", 12},
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
