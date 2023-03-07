#include "ipfs_client/block_storage.h"
#include "ipfs_client/unixfs_path_resolver.h"

#include "vocab/log_macros.h"
#include "vocab/stringify.h"

bool ipfs::BlockStorage::Store(std::shared_ptr<FrameworkApi> api,
                               std::string const& cid,
                               ipfs::Block&& block) {
  // TODO validate, return false if fail
  if (cid2node_.emplace(cid, std::move(block)).second == false) {
    return false;  // We've already seen this block
  }
  LOG(INFO) << "Stored a block of type: " << ipfs::Stringify(block.type())
            << ' ' << cid;
  for (UnixFsPathResolver* ptr : listening_) {
    LOG(INFO) << "A resolver was waiting on " << ptr->waiting_on();
    if (ptr->waiting_on() == cid) {
      ptr->Step(api);
    }
  }
  CheckDoneListening();
  return true;
}
ipfs::Block const* ipfs::BlockStorage::Get(std::string const& cid) const {
  auto it = cid2node_.find(cid);
  if (it == cid2node_.end()) {
    //    L_WRN("Sorry, I don't have " << cid << " already stored.");
    return nullptr;
  }
  return &(it->second);
}

void ipfs::BlockStorage::AddListening(UnixFsPathResolver* p) {
  listening_.insert(p);
}
void ipfs::BlockStorage::StopListening(UnixFsPathResolver* p) {
  auto e = std::remove(listening_.begin(), listening_.end(), p);
  listening_.erase(e, listening_.end());
}
void ipfs::BlockStorage::CheckDoneListening() {
  while (true) {
    auto done_it =
        std::find_if(listening_.begin(), listening_.end(),
                     [](auto& p) { return p->waiting_on().empty(); });
    if (done_it == listening_.end()) {
      return;
    }
    listening_.erase(done_it);
  }
}

ipfs::BlockStorage::BlockStorage() {}
ipfs::BlockStorage::~BlockStorage() noexcept {}
