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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 1036},
      {"https://ipfs.io/", 977},
      {"https://dweb.link/", 949},
      {"https://trustless-gateway.link/", 945},
      {"https://hardbin.com/", 924},
      {"https://ipfs.greyh.at/", 865},
      {"https://ipfs.joaoleitao.org/", 861},
      {"https://dlunar.net/", 702},
      {"https://flk-ipfs.io/", 686},
      {"https://ipfs.cyou/", 477},
      {"https://human.mypinata.cloud/", 418},
      {"https://jcsl.hopto.org/", 375},
      {"https://delegated-ipfs.dev/", 330},
      {"https://4everland.io/", 303},
      {"https://ipfs.runfission.com/", 274},
      {"https://gateway.pinata.cloud/", 147},
      {"https://dag.w3s.link/", 141},
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
