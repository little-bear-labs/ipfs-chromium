#include "ipfs_client/block_storage.h"
#include "ipfs_client/unixfs_path_resolver.h"

#include <libp2p/multi/content_identifier_codec.hpp>

#include "log_macros.h"
#include "vocab/stringify.h"

using Codec = libp2p::multi::ContentIdentifierCodec;

bool ipfs::BlockStorage::Store(std::string cid_str,
                               ipfs::Cid const&,
                               std::string headers,
                               Block&& block) {
  LOG(INFO) << "Store(" << cid_str << ')';
  auto t = std::time(nullptr);
  auto it = cid2record_.find(cid_str);
  if (it != cid2record_.end()) {
    if (it->second) {
      LOG(INFO) << cid_str << " already stored.";
      it->second->last_access = t;
      return false;
    } else {
      LOG(INFO) << "Filling in a null placeholder.";
    }
  }
  auto* into = FindFree(t);
  cid2record_[cid_str] = into;
  if (into->cid_str.size()) {
    LOG(ERROR) << "Evicting " << into->cid_str << " to make room for "
               << cid_str;
    cid2record_.erase(into->cid_str);
  }
  into->cid_str = cid_str;
  into->last_access = t;
  into->block = std::move(block);
  into->headers = std::move(headers);
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
auto ipfs::BlockStorage::GetInternal(std::string const& cid, bool deep)
    -> Record const* {
  auto it = cid2record_.find(cid);
  if (it == cid2record_.end() && deep && checking_.insert(cid).second) {
    LOG(INFO) << "Check serialized cache for " << cid;
    if (cache_search_initiator_) {
      cache_search_initiator_(cid);
    }
    it = cid2record_.find(cid);
    checking_.erase(cid);
  }
  if (it == cid2record_.end()) {
    LOG(INFO) << "Data not found in immediate object storage for " << cid
              << ", deep" << deep;
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
  LOG(INFO) << "L0: " << cid << " deep=" << deep << " " << rec->last_access;
  return rec;
}
ipfs::Block const* ipfs::BlockStorage::Get(std::string const& cid, bool deep) {
  LOG(INFO) << "Get(" << cid << ',' << deep << ')';
  auto* result = GetInternal(cid, deep);
  if (result) {
    return &(result->block);
  }
  return nullptr;
}
std::string const* ipfs::BlockStorage::GetHeaders(const std::string& cid) {
  LOG(INFO) << "GetHeaders(" << cid << ')';
  auto* result = GetInternal(cid, true);
  if (result) {
    return &(result->headers);
  }
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
  auto looking = true;
  while (looking) {
    looking = false;
    for (UnixFsPathResolver* ptr : listening_) {
      auto cid = ptr->current_cid();
      L_VAR(cid);
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
}
auto ipfs::BlockStorage::FindFree(std::time_t now) -> Record* {
  if (records_.size() < 8UL) {
    return Allocate();
  }
  for (auto i = 0; i < 9; ++i) {
    if (reuse_ == records_.end()) {
      reuse_ = records_.begin();
      continue;
    }
    auto it = reuse_;
    std::advance(reuse_, 1);
    if (now - it->last_access > 9) {
      LOG(INFO) << it->last_access << " is too old.";
      it->last_access = now;
      return &*it;
    }
  }
  return Allocate();
}
auto ipfs::BlockStorage::Allocate() -> Record* {
  LOG(INFO) << "Expanding store size to " << (records_.size() + 1UL);
  return &records_.emplace_back();
}

void ipfs::BlockStorage::cache_search_initiator(
    std::function<void(std::string)> f) {
  cache_search_initiator_ = f;
}

ipfs::BlockStorage::BlockStorage() {}

ipfs::BlockStorage::~BlockStorage() noexcept {
  LOG(WARNING) << "BlockStorage dtor!";
}
ipfs::BlockStorage::Record::Record() = default;
ipfs::BlockStorage::Record::~Record() noexcept = default;
