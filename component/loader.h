#ifndef COMPONENTS_IPFS_URL_LOADER_H_
#define COMPONENTS_IPFS_URL_LOADER_H_ 1

#include "services/network/url_loader.h"

namespace ipfs {

  class Loader final : public network::URLLoader {
  public:
    Loader( network::URLLoaderContext& ctxt, network::ResourceRequest const& res_req );
  };

}

#endif
