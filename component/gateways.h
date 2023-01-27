#ifndef CHROMIUM_IPFS_GATEWAYS_H_
#define CHROMIUM_IPFS_GATEWAYS_H_

#include "base/containers/flat_map.h"

#include <string_view>

namespace ipfs {
    class Gateways
    {
        base::flat_map<std::string,unsigned> known_gateways_;
    public:
        static Gateways& instance();//TODO proper lifetime & ownership

        std::vector<std::string> get_list() const;
        void promote(std::string_view);
        void  demote(std::string_view);

    private:
        Gateways();
        ~Gateways();
    };
}

#endif // CHROMIUM_IPFS_GATEWAYS_H_
