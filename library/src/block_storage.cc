#include "ipfs_client/block_storage.h"
#include "ipfs_client/unixfs_path_resolver.h"

using Self = ipfs::BlockStorage;
using Node = ipfs::Block;

bool Self::Store(std::string const& cid, Node&& block) {
  // TODO validate, return false if fail
  if (cid2node_.emplace(cid, std::move(block)).second == false) {
    return false;  // We've already seen this block
  }
  for (auto& ptr : listening_) {
    if (ptr->waiting_on() == cid) {
      ptr->Step(ptr);
    }
  }
  auto new_end =
      std::remove_if(listening_.begin(), listening_.end(),
                     [](auto& p) { return p->waiting_on().empty(); });
  listening_.erase(new_end, listening_.end());
  return true;
}
auto Self::Get(std::string const& cid) const -> Node const* {
  auto it = cid2node_.find(cid);
  if (it == cid2node_.end()) {
    return nullptr;
  }
  return &(it->second);
}

void ipfs::BlockStorage::AddListening(std::shared_ptr<UnixFsPathResolver> p) {
  listening_.insert(p);
}

Self::BlockStorage() {}
Self::~BlockStorage() noexcept {}
