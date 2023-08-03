#ifndef IPFS_IDENTITY_CID_H_
#define IPFS_IDENTITY_CID_H_ 1

#include <libp2p/multi/content_identifier.hpp>

#include <string_view>

namespace ipfs {
namespace id_cid {
using Cid = libp2p::multi::ContentIdentifier;

Cid forText(std::string_view);
}  // namespace id_cid
}  // namespace ipfs

#endif
