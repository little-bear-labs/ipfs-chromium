#include "ipfs_client/gateways.h"

#include <string>

using namespace std::string_literals;

ipfs::Gateways::Gateways()
: known_gateways_{
      {"http://localhost:8080/"s, 9}
    , {"https://4everland.io/"s, 0}
    , {"https://c4rex.co/"s, 0}
    , {"https://cloudflare-ipfs.com/"s, 0}
    , {"https://dweb.link/"s, 1}
    , {"https://gateway.ipfs.io/"s, 1}
    , {"https://gateway.pinata.cloud/"s, 0}
    , {"https://ipfs.czip.it/"s, 1}
    , {"https://ipfs.best-practice.se/"s, 1}
    , {"https://ipfs-gateway.cloud/"s, 1}
    , {"https://ipfs.io/"s, 1}
    , {"https://ipfs.joaoleitao.org/"s, 2}
    , {"https://ipfs.storry.tv/"s, 1}
    , {"https://jorropo.net/"s, 1}
    , {"https://ipfs.jpu.jp/"s, 1}
    , {"https://ipfs.litnet.work/"s, 1}
    , {"https://ipfs.runfission.com/"s, 0}
    , {"https://jorropo.net/"s, 0}
    , {"https://hardbin.com/"s, 0}
    , {"https://nftstorage.link/"s, 0}
    , {"https://video.oneloveipfs.com/"s, 0}
    , {"https://cf-ipfs.com/"s, 0}
    , {"https://storry.tv/"s, 1}
    , {"https://via0.com/"s, 0}
    , {"https://w3s.link/"s, 0}
}
, random_engine_{std::random_device{}()}
, dist_{0.0625}
{
}
ipfs::Gateways::~Gateways() {}

auto ipfs::Gateways::GenerateList() const -> GatewayList {
    GatewayList result;
    for ( auto [k,v] : known_gateways_ ) {
        result.emplace(k,v + dist_(random_engine_));
    }
    return result;
}

void ipfs::Gateways::promote(std::string const& key) {
    known_gateways_.at(key)++;
}
void ipfs::Gateways::demote(std::string const& key) {
    auto prio = known_gateways_.at(key);
    if (prio > 1) {
        --prio;
    }
}
