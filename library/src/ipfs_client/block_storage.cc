#include "ipfs_client/block_storage.h"
#include "ipfs_client/unixfs_path_resolver.h"

#include <libp2p/multi/content_identifier_codec.hpp>

#include "log_macros.h"
#include "vocab/stringify.h"

using Codec = libp2p::multi::ContentIdentifierCodec;

bool ipfs::BlockStorage::Store(std::string cid_str,
                               ipfs::Cid const&,
                               std::string headers,
                               std::string const& body,
                               Block&& block) {
  LOG(INFO) << "Store(" << cid_str << ')';
  auto t = std::time(nullptr);
  auto it = cid2record_.find(cid_str);
  if (it != cid2record_.end()) {
    if (it->second) {
      LOG(INFO) << cid_str << " already stored.";
      it->second->last_access = t;
      CheckListening();
      return false;
    } else {
      LOG(INFO) << "Filling in a null placeholder.";
    }
  }
  auto* into = FindFree(t);
  cid2record_[cid_str] = into;
  if (into->cid_str.empty()) {
    LOG(INFO) << "Storing " << cid_str << " in fresh node @" << (void*)into;
  } else {
    LOG(INFO) << "Evicting " << into->cid_str << " to make room for "
              << cid_str;
    cid2record_.erase(into->cid_str);
  }
  into->cid_str = cid_str;
  into->last_access = t;
  into->block = std::move(block);
  into->headers = std::move(headers);
  if (into->headers.size() && body.size()) {
    for (auto& hook : hooks_) {
      hook(cid_str, into->headers, body);
    }
  }
  CheckListening();
  return true;
}
bool ipfs::BlockStorage::Store(const ipfs::Cid& cid,
                               std::string headers,
                               std::string const& body,
                               ipfs::Block&& block) {
  auto cid_val = Codec::toString(cid);
  if (cid_val.has_value()) {
    return Store(cid_val.value(), cid, headers, body, std::move(block));
  } else {
    return false;
  }
}
bool ipfs::BlockStorage::Store(std::string headers,
                               std::string const& body,
                               ipfs::Block&& block) {
  return Store(block.cid(), headers, body, std::move(block));
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
  return Store(cid_str, cid, headers, body, {cid, body});
}
auto ipfs::BlockStorage::GetInternal(std::string const& cid) -> Record const* {
  auto it = cid2record_.find(cid);
  /*
  if (it == cid2record_.end() && deep) {
    LOG(INFO) << "Check serialized cache for " << cid;
    if (cache_search_initiator_) {
      cache_search_initiator_(cid);
    }
    it = cid2record_.find(cid);
  }
  */
  if (it == cid2record_.end()) {
    VLOG(1) << "Data not found in immediate object storage for " << cid;
    return nullptr;
  }
  auto* rec = it->second;
  if (!rec) {
    LOG(INFO) << "Placeholder null had been stored for " << cid;
    return nullptr;
  }
  if (cid != rec->cid_str) {
    LOG(FATAL) << cid << " mapped into entry with CID " << rec->cid_str;
  }
  rec->last_access = std::time(nullptr);
  return rec;
}
ipfs::Block const* ipfs::BlockStorage::Get(std::string const& cid, bool) {
  VLOG(1) << "Get(" << cid << ')';
  auto* result = GetInternal(cid);
  if (result) {
    return &(result->block);
  }
  return nullptr;
}
std::string const* ipfs::BlockStorage::GetHeaders(const std::string& cid) {
  auto* result = GetInternal(cid);
  if (result) {
    VLOG(1) << "GetHeaders(" << cid << ")->size()=" << result->headers.size();
    return &(result->headers);
  }
  LOG(ERROR) << "Found no headers for " << cid << "!";
  return nullptr;
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
  if (checking_) {
    return;
  }
  checking_ = true;
  auto looking = true;
  while (looking) {
    looking = false;
    for (UnixFsPathResolver* ptr : listening_) {
      auto cid = ptr->current_cid();
      if (cid.empty()) {
        LOG(INFO) << "Kicking a listener out.";
        looking = true;
        auto prev = ptr->MaybeGetPreviousListener();
        listening_.erase(ptr);
        if (prev) {
          ptr->Step(prev);
        }
        break;
      }
      auto it = cid2record_.find(cid);
      if (it != cid2record_.end() && it->second && it->second->cid_str == cid) {
        auto prev = ptr->MaybeGetPreviousListener();
        if (prev) {
          LOG(INFO) << "Stepping listener of (" << cid << ")";
          ptr->Step(prev);
          looking = true;
        }
      }
    }
  }
  checking_ = false;
}
auto ipfs::BlockStorage::FindFree(std::time_t now) -> Record* {
  DCHECK(records_.size() > 99);
  std::list<Record> l;
  l.splice(l.end(), records_, records_.begin());
  records_.splice(records_.end(), l);
  if (now - records_.back().last_access > 300) {
    LOG(INFO) << records_.back().last_access << " is too old.";
    return &records_.back();
  }
  LOG(INFO) << "Not ready to kick out @ " << (void*)&records_.back()
            << " yet: " << records_.back().last_access;
  if (now - records_.front().last_access > 300) {
    return FindFree(now);
  }
  return Allocate();
}
auto ipfs::BlockStorage::Allocate() -> Record* {
  LOG(INFO) << "Expanding store size to " << (records_.size() + 1UL);
  records_.emplace_back();
  return &records_.back();
}
void ipfs::BlockStorage::AddStorageHook(SerializedStorageHook h) {
  hooks_.push_back(h);
}

ipfs::BlockStorage::BlockStorage() {}
ipfs::BlockStorage::~BlockStorage() noexcept {
  LOG(WARNING) << "BlockStorage dtor!";
}

ipfs::BlockStorage::Record::Record() = default;
ipfs::BlockStorage::Record::~Record() noexcept = default;
