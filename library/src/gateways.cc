#include "ipfs_client/gateways.h"

#include <string>

using namespace std::string_literals;

ipfs::Gateways::Gateways()
: known_gateways_{
      {"http://localhost:8080/"s, 19}
    , {"https://4everland.io/"s, 10}
    , {"https://c4rex.co/"s, 10}
    , {"https://cloudflare-ipfs.com/"s, 10}
    , {"https://dweb.link/"s, 11}
    , {"https://gateway.ipfs.io/"s, 11}
    , {"https://gateway.pinata.cloud/"s, 10}
    , {"https://ipfs.czip.it/"s, 11}
    , {"https://ipfs.best-practice.se/"s, 11}
    , {"https://ipfs-gateway.cloud/"s, 11}
    , {"https://ipfs.io/"s, 11}
    , {"https://ipfs.joaoleitao.org/"s, 12}
    , {"https://ipfs.storry.tv/"s, 11}
    , {"https://jorropo.net/"s, 11}
    , {"https://ipfs.jpu.jp/"s, 11}
    , {"https://ipfs.litnet.work/"s, 11}
    , {"https://ipfs.runfission.com/"s, 10}
    , {"https://jorropo.net/"s, 10}
    , {"https://hardbin.com/"s, 10}
    , {"https://nftstorage.link/"s, 10}
    , {"https://video.oneloveipfs.com/"s, 10}
    , {"https://cf-ipfs.com/"s, 10}
    , {"https://storry.tv/"s, 11}
    , {"https://via0.com/"s, 10}
    , {"https://w3s.link/"s, 10}
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
