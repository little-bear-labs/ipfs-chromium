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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 1026},
      {"https://ipfs.io/", 986},
      {"https://dweb.link/", 959},
      {"https://hardbin.com/", 941},
      {"https://trustless-gateway.link/", 928},
      {"https://ipfs.joaoleitao.org/", 876},
      {"https://ipfs.greyh.at/", 875},
      {"https://dlunar.net/", 719},
      {"https://flk-ipfs.io/", 703},
      {"https://ipfs.cyou/", 487},
      {"https://human.mypinata.cloud/", 428},
      {"https://jcsl.hopto.org/", 389},
      {"https://delegated-ipfs.dev/", 339},
      {"https://4everland.io/", 313},
      {"https://ipfs.runfission.com/", 283},
      {"https://gateway.pinata.cloud/", 156},
      {"https://dag.w3s.link/", 150},
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
