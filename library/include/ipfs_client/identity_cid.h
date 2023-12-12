#ifndef IPFS_IDENTITY_CID_H_
#define IPFS_IDENTITY_CID_H_ 1

#include <ipfs_client/cid.h>

#include <string_view>

namespace ipfs {
namespace id_cid {
ipfs::Cid forText(std::string_view);
}  // namespace id_cid
}  // namespace ipfs

#endif
