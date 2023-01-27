#include "gateways.h"

#include <string>

using Self = ipfs::Gateways;

auto Self::instance() -> Gateways& {
    static Gateways the;
    return the;
}

std::vector<std::string> Self::get_list() const
{
    //TODO - this whole class
    using namespace std::string_literals;
    return {
          "http://localhost:8080/"s
        , "https://ipfs.io/"s
        , "https://cloudflare-ipfs.com/"s
        , "https://gateway.pinata.cloud/"s
        , "https://gateway.ipfs.io/"s
        , "https://dweb.link/"s
        , "https://ipfs.runfission.com/"s
        , "https://ipfs-gateway.cloud/"s
        , "https://w3s.link/"s
        , "https://via0.com/"s
        , "https://jorropo.net/"s
        , "https://hardbin.com/"s
        , "https://nftstorage.link/"s
        , "https://video.oneloveipfs.com/"s
        , "https://cf-ipfs.com/"s
        , "4everland.io"s
    };
}

Self::Gateways() {}
Self::~Gateways() {}
