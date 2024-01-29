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
  while (user_override >> gw) {
    if (gw.empty()) {
      continue;
    }
    if (gw.back() != '/') {
      gw.push_back('/');
    }
    cfg.AddGateway(gw);
  }
  return true;
}

void ctx::LoadStaticGatewayList(ipfs::ctx::GatewayConfig& cfg) {
  auto static_list = {
      std::pair<std::string_view, int>{"http://localhost:8080/", 12'100},
      {"https://ipfs.io/", 751},
      {"https://dag.w3s.link/", 213},
      {"https://gateway.ipfs.io/", 589},
      {"https://jcsl.hopto.org/", 695},
      {"https://ipfs.runfission.com/", 119},
      {"https://delegated-ipfs.dev/", 75},
      {"https://human.mypinata.cloud/", 305},
      {"https://cesginc.com/", 50},
      {"https://permaweb.eu.org/", 54},
      {"https://dweb.link/", 53},
      {"https://http.f02620.devtty.eu/", 50},
      {"https://f010479.twinquasar.io/", 36},
      {"https://ipfs.joaoleitao.org/", 34},
      {"https://gateway.pinata.cloud/", 15},
      {"http://f02095132.datasetcreators.com/", 13},
      {"https://nftstorage.link/", 7},
      {"https://ipfs.omnicloudstorage.com:9443/", 6},
      {"https://ipfs.fleek.co/", 5},
      {"https://data.filstorage.io/", 4},
      {"https://w3s.link/", 3},
      {"https://hardbin.com/", 2},
      {"https://ipfs.jpu.jp/", 1},
      {"https://jorropo.net/", 0},
  };
  for (auto [gw, rt] : static_list) {
    cfg.AddGateway(gw);
    cfg.SetGatewayRate(gw, rt);
  }
}
