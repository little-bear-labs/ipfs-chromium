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
      std::pair<std::string_view, int>{"http://127.0.0.1:8080/", 12'036},
      {"https://jcsl.hopto.org/", 441},
      {"https://ipfs.io/", 409},
      {"https://gateway.ipfs.io/", 383},
      {"https://ipfs.eth.aragon.network/", 289},
      {"https://cloudflare-ipfs.com/", 210},
      {"https://ipfs.greyh.at/", 148},
      {"https://cf-ipfs.com/", 93},
      {"https://ipfs.joaoleitao.org/", 51},
      {"https://trustless-gateway.link/", 18},
      {"https://ipfs.fleek.co/", 16},
      {"https://hardbin.com/", 12},
      {"https://permaweb.eu.org/", 11},
      {"https://nftstorage.link/", 9},
      {"https://w3s.link/", 9},
      {"https://4everland.io/", 9},
      {"https://human.mypinata.cloud/", 6},
      {"https://dweb.link/", 5},
      {"https://ipfs.runfission.com/", 5},
      {"https://ipfs.omnicloudstorage.com:9443/", 5},
      {"https://data.filstorage.io/", 4},
      {"https://http.f02620.devtty.eu/", 4},
      {"https://cesginc.com/", 3},
      {"https://dag.w3s.link/", 2},
      {"https://delegated-ipfs.dev/", 2},
      {"https://gateway.pinata.cloud/", 1},
      {"https://f010479.twinquasar.io/", 0}};
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw, rt);
  }
}
