#include <ipfs_client/ctx/default_gateways.h>

#include <ipfs_client/ctx/gateway_config.h>

#include <cstdlib>
#include <sstream>
#include <string>
#include <utility>
#include <string_view>

namespace ctx = ipfs::ctx;

auto ctx::LoadGatewaysFromEnvironmentVariable(ipfs::ctx::GatewayConfig& cfg) -> bool {
  auto* ovr = std::getenv("IPFS_GATEWAY");
  if (ovr == nullptr) {
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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 1039},
      {"https://ipfs.io/", 1009},
      {"https://dweb.link/", 946},
      {"https://trustless-gateway.link/", 942},
      {"https://hardbin.com/", 918},
      {"https://ipfs.greyh.at/", 862},
      {"https://ipfs.joaoleitao.org/", 856},
      {"https://dlunar.net/", 697},
      {"https://flk-ipfs.io/", 681},
      {"https://ipfs.cyou/", 474},
      {"https://human.mypinata.cloud/", 415},
      {"https://jcsl.hopto.org/", 369},
      {"https://delegated-ipfs.dev/", 324},
      {"https://4everland.io/", 300},
      {"https://ipfs.runfission.com/", 268},
      {"https://gateway.pinata.cloud/", 144},
      {"https://dag.w3s.link/", 138},
      {"https://flk-ipfs.xyz/", 100},
      {"https://ipfs.eth.aragon.network/", 14},
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
