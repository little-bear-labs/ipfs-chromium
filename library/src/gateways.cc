#include "ipfs_client/gateways.h"

#include <string>

using namespace std::string_literals;

ipfs::Gateways::Gateways()
: known_gateways_{
      {"http://localhost:8080/"s, 0}
    , {"https://ipfs.io/"s, 0}
//    , {"https://cloudflare-ipfs.com/"s, 0}
    , {"https://gateway.pinata.cloud/"s, 0}
    , {"https://gateway.ipfs.io/"s, 0}
    , {"https://dweb.link/"s, 0}
    , {"https://ipfs.runfission.com/"s, 0}
    , {"https://ipfs-gateway.cloud/"s, 0}
//    , {"https://w3s.link/"s, 0}
    , {"https://via0.com/"s, 0}
    , {"https://jorropo.net/"s, 0}
    , {"https://hardbin.com/"s, 0}
    , {"https://nftstorage.link/"s, 0}
    , {"https://video.oneloveipfs.com/"s, 0}
    , {"https://cf-ipfs.com/"s, 0}
    , {"https://4everland.io/"s, 0}
    }
, random_engine_{std::random_device{}()}
, dist_{0.0625}
{
}
ipfs::Gateways::~Gateways() {}

auto ipfs::Gateways::get_list() const -> GatewayList {
    GatewayList result;
    for ( auto [k,v] : known_gateways_ ) {
        result.emplace(k,v + dist_(random_engine_));
    }
    return result;
}

ipfs::Gateway::Gateway(std::string url_prefix, unsigned int priority)
: prefix_{std::move(url_prefix)}
, priority_{priority}
{}
ipfs::Gateway::Gateway(Gateway const& other)
: prefix_{other.prefix_}
, priority_{other.priority_}
{}
ipfs::Gateway::~Gateway(){}

//Less means should-be-preferred
bool ipfs::Gateway::operator<(Gateway const& rhs) const {
    if ( failed_requests_.size() != rhs.failed_requests_.size() ) {
        return failed_requests_.size() < rhs.failed_requests_.size();
    }
    if ( priority_ != rhs.priority_ ) {
        return priority_ > rhs.priority_;
    }
    return prefix_ < rhs.prefix_;
}
bool ipfs::Gateway::accept(const std::string& suffix) const {
    return ! failed_requests_.contains(suffix);
}