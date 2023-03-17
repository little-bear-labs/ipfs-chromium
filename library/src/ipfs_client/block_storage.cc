#include "ipfs_client/block_storage.h"
#include "ipfs_client/unixfs_path_resolver.h"

#include <libp2p/multi/content_identifier_codec.hpp>

#include "vocab/log_macros.h"
#include "vocab/stringify.h"

bool ipfs::BlockStorage::Store(std::shared_ptr<DagListener> listener,
                               std::string const& cid,
                               ipfs::Block&& block) {
  if (cid2node_.emplace(cid, std::move(block)).second == false) {
    return false;  // We've already seen this block
  }
  LOG(INFO) << "Stored a block of type: " << ipfs::Stringify(block.type())
            << ' ' << cid;
  for (UnixFsPathResolver* ptr : listening_) {
    LOG(INFO) << "A resolver was waiting on " << ptr->waiting_on();
    if (ptr->waiting_on() == cid) {
      ptr->Step(listener);
    }
  }
  CheckDoneListening();
  return true;
}
bool ipfs::BlockStorage::Store(std::shared_ptr<DagListener> api,
                               Block&& block) {
  auto cid_res = libp2p::multi::ContentIdentifierCodec::toString(block.cid());
  DCHECK(cid_res.has_value());
  return Store(api, cid_res.value(), std::move(block));
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
  LOG(INFO) << "AddListening(" << p->waiting_on() << ')';
  listening_.insert(p);
}

void ipfs::BlockStorage::StopListening(UnixFsPathResolver* p) {
  LOG(INFO) << "StopListening(" << p->waiting_on() << ')';
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
    LOG(INFO) << "Kicking this listener out: " << (*done_it)->waiting_on();
    listening_.erase(done_it);
  }
}

ipfs::BlockStorage::BlockStorage() {}

ipfs::BlockStorage::~BlockStorage() noexcept {
  LOG(WARNING) << "BlockStorage dtor!";
}
