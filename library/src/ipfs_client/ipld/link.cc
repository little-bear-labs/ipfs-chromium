#include "ipfs_client/ipld/link.h"

#include <utility>
#include <string>
#include <memory>

using Self = ipfs::ipld::Link;

Self::Link(std::string cid_s) : cid{std::move(cid_s)} {}
Self::Link(std::string s, std::shared_ptr<DagNode> n) : cid{std::move(s)}, node{std::move(n)} {}
