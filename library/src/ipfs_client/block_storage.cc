#include "ipfs_client/block_storage.h"
#include "ipfs_client/unixfs_path_resolver.h"

#include <libp2p/multi/content_identifier_codec.hpp>

#include "log_macros.h"
#include "vocab/stringify.h"

using Codec = libp2p::multi::ContentIdentifierCodec;

bool ipfs::BlockStorage::Store(std::string cid_str,
                               ipfs::Cid const&,
                               std::string /*TODO headers*/,
                               Block&& block) {
  if (cid2node_.count(cid_str)) {
    return false;
  }
  auto& n = nodes_[index_];
  if (n.valid()) {
    auto old_cid = Codec ::toString(n.cid());
    if (old_cid.has_value()) {
      LOG(ERROR) << "Evicting " << old_cid.value() << " to make room for "
                 << cid_str;
      cid2node_.erase(old_cid.value());
    }
  }
  n = std::move(block);
  cid2node_[cid_str] = &n;
  index_ = (index_ + 1) % nodes_.size();
  CheckListening();
  return true;
}
bool ipfs::BlockStorage::Store(const ipfs::Cid& cid,
                               std::string headers,
                               ipfs::Block&& block) {
  auto cid_val = Codec::toString(cid);
  if (cid_val.has_value()) {
    return Store(cid_val.value(), cid, headers, std::move(block));
  } else {
    return false;
  }
}
bool ipfs::BlockStorage::Store(std::string headers, ipfs::Block&& block) {
  return Store(block.cid(), headers, std::move(block));
}
bool ipfs::BlockStorage::Store(std::string const& cid,
                               std::string headers,
                               std::string body) {
  LOG(INFO) << "Store(cid=" << cid << " headers.size()=" << headers.size()
            << " body.size()=" << body.size() << ')';
  DCHECK(headers != body);
  auto cid_res = Codec::fromString(cid);
  DCHECK(cid_res.has_value());
  return Store(cid, cid_res.value(), headers, body);
}
bool ipfs::BlockStorage::Store(std::string cid_str,
                               const ipfs::Cid& cid,
                               std::string headers,
                               std::string body) {
  LOG(INFO) << "Store(cid=" << cid_str
            << ", <cid obj>, headers.size()=" << headers.size()
            << " body.size()=" << body.size() << ')';
  return Store(cid_str, cid, headers, {cid, body});
}
ipfs::Block const* ipfs::BlockStorage::Get(std::string const& cid) const {
  LOG(INFO) << "hit 1 : storage @ " << (void*)this << ", map@"
            << (void*)(&cid2node_);
  auto it = cid2node_.find(cid);
  if (it == cid2node_.end()) {
    if (cache_search_initiator_) {
      cache_search_initiator_(cid);
    }
    LOG(INFO) << "hit 2 : storage @ " << (void*)this << ", map@"
              << (void*)(&cid2node_);
    it = cid2node_.find(cid);
  }
  if (it == cid2node_.end()) {
    return nullptr;
  }
  LOG(INFO) << "L0: " << cid;
  return it->second;
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
        auto prev = ptr->MaybeGetPreviousListener();
        if (prev) {
          ptr->Step(prev);
        }
      }
    }
  }
}

void ipfs::BlockStorage::cache_search_initiator(
    std::function<void(std::string)> f) {
  cache_search_initiator_ = f;
}

ipfs::BlockStorage::BlockStorage() {}

ipfs::BlockStorage::~BlockStorage() noexcept {
  LOG(WARNING) << "BlockStorage dtor!";
}
