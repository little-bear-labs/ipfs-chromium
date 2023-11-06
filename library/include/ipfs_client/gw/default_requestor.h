#ifndef IPFS_DEFAULT_REQUESTOR_LIST_H_
#define IPFS_DEFAULT_REQUESTOR_LIST_H_

#include "requestor.h"

#include <ipfs_client/gateways.h>

namespace ipfs::gw {
std::shared_ptr<Requestor> default_requestor(GatewayList,
                                             std::shared_ptr<ContextApi>);
}

#endif  // IPFS_DEFAULT_REQUESTOR_LIST_H_
