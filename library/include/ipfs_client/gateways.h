#ifndef CHROMIUM_IPFS_GATEWAYS_H_
#define CHROMIUM_IPFS_GATEWAYS_H_

#include "vocab/flat_mapset.h"

#include <random>
#include <string_view>
#include <string>
#include <vector>

namespace ipfs {
    class Gateway {
      std::string prefix_;
      flat_set<std::string> failed_requests_;
      unsigned priority_;
      std::string tasked_with_;

    public:
      Gateway(std::string url_prefix, unsigned priority);
      explicit Gateway(Gateway const&);
      ~Gateway();
      bool operator<(Gateway const&) const;
      bool accept(std::string const& suffix);
      std::string const& url_prefix() const;
      void make_available();
      std::string url() const;
    };

    using GatewayList = flat_set<Gateway>;

    class Gateways {
        flat_map<std::string,unsigned> known_gateways_;
        mutable std::default_random_engine random_engine_;//TODO - no way mutable is allowed to slide
        mutable std::geometric_distribution<unsigned> dist_;
    public:
        Gateways();
        ~Gateways();
        GatewayList get_list() const;
        void promote(std::string_view);
        void  demote(std::string_view);
    };
}

#endif // CHROMIUM_IPFS_GATEWAYS_H_
