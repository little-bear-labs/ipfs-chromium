#ifndef IPFS_DEFAULT_REQUESTOR_LIST_H_
#define IPFS_DEFAULT_REQUESTOR_LIST_H_

#include "requestor.h"

namespace ipfs::gw {
std::shared_ptr<Requestor> default_requestor(std::shared_ptr<Requestor> early,
                                             std::shared_ptr<Client>);
}

#endif  // IPFS_DEFAULT_REQUESTOR_LIST_H_
