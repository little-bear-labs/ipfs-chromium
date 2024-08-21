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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 1001},
      {"https://ipfs.io/", 991},
      {"https://dweb.link/", 984},
      {"https://hardbin.com/", 968},
      {"https://ipfs.joaoleitao.org/", 903},
      {"https://ipfs.greyh.at/", 902},
      {"https://trustless-gateway.link/", 789},
      {"https://human.mypinata.cloud/", 453},
      {"https://delegated-ipfs.dev/", 362},
      {"https://jcsl.hopto.org/", 356},
      {"https://4everland.io/", 338},
      {"https://ipfs.runfission.com/", 307},
      {"https://gateway.pinata.cloud/", 204},
      {"https://dag.w3s.link/", 177},
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
