#include "ipfs_client/block_storage.h"
#include "ipfs_client/unixfs_path_resolver.h"

#include <libp2p/multi/content_identifier_codec.hpp>

#include "log_macros.h"
#include "vocab/stringify.h"

bool ipfs::BlockStorage::Store(std::string const& cid, ipfs::Block&& block) {
  if (cid2node_.emplace(cid, std::move(block)).second == false) {
    return false;  // We've already seen this block
  }
  //  LOG(INFO) << "Stored a block of type: " << ipfs::Stringify(block.type())
  //  << ' ' << cid;
  CheckListening();
  return true;
}
bool ipfs::BlockStorage::Store(Block&& block) {
  auto cid_res = libp2p::multi::ContentIdentifierCodec::toString(block.cid());
  DCHECK(cid_res.has_value());
  return Store(cid_res.value(), std::move(block));
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
  //  LOG(INFO) << "AddListening(" << p->current_cid() << ')';
  listening_.insert(p);
}

void ipfs::BlockStorage::StopListening(UnixFsPathResolver* p) {
  LOG(INFO) << "StopListening(" << p->current_cid() << ')';
  auto e = std::remove(listening_.begin(), listening_.end(), p);
  listening_.erase(e, listening_.end());
}

void ipfs::BlockStorage::CheckListening() {
  auto looking = true;
  while (looking) {
    looking = false;
    for (UnixFsPathResolver* ptr : listening_) {
      auto cid = ptr->current_cid();
      if (cid.empty()) {
        LOG(INFO) << "Kicking a listener out.";
        looking = true;
        auto prev = ptr->MaybeGetPreviousListener();
        if (prev) {
          ptr->Step(prev);
        }
        listening_.erase(ptr);
        break;
      }
      if (Get(cid)) {
        LOG(INFO) << "A resolver was waiting on " << cid
                  << " which is now available.";
        auto prev = ptr->MaybeGetPreviousListener();
        if (prev) {
          ptr->Step(prev);
        }
      }
    }
  }
}
ipfs::BlockStorage::BlockStorage() {}

ipfs::BlockStorage::~BlockStorage() noexcept {
  LOG(WARNING) << "BlockStorage dtor!";
}
