#include "ipfs_client/ipld/link.h"

using Self = ipfs::ipld::Link;

Self::Link(std::string cid_s) : cid{cid_s} {}
Self::Link(std::string s, std::shared_ptr<DagNode> n) : cid{s}, node{n} {}
