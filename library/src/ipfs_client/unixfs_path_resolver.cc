#include "ipfs_client/unixfs_path_resolver.h"

#include "unix_fs/node_helper.h"

#include <ipfs_client/block_storage.h>
#include <ipfs_client/context_api.h>
#include <ipfs_client/dag_block.h>
#include <libp2p/multi/content_identifier_codec.hpp>

#include "vocab/stringify.h"

#include "log_macros.h"

using Self = ipfs::UnixFsPathResolver;

void Self::Step(std::shared_ptr<DagListener> listener) {
  old_listener_ = listener;
  if (cid_.empty()) {
    return;
  }
  LOG(INFO) << "Stepping... " << cid_ << " / " << original_path_ << " / "
            << path_;
  if (involved_cids_.end() ==
      std::find(involved_cids_.begin(), involved_cids_.end(), cid_)) {
    involved_cids_.push_back(cid_);
  }
  Block const* block = storage_.Get(cid_, true);
  if (!block) {
    LOG(INFO) << "Current block " << cid_ << " not found. Requesting.";
    Request(listener, cid_, prio_);
    return;
  }
  if (!block->valid() || block->type() == Block::Type::Invalid) {
    LOG(ERROR) << "Encountered broken block!! " << cid_;
    listener->FourOhFour(cid_, original_path_);
    return;
  }
  if (!helper_) {
    GetHelper(block->type());
  }
  if (!helper_) {
    listener->FourOhFour(cid_, original_path_);
    return;
  }
  auto requestor = [this, &listener](std::string cid, Priority prio) {
    this->Request(listener, cid, prio);
  };
  using Codec = libp2p::multi::ContentIdentifierCodec;
  DCHECK_EQ(cid_, Codec::toString(block->cid()).value());
  std::unique_ptr<unix_fs::NodeHelper> helper;
  if (helper_->Process(helper, listener, requestor, cid_)) {
    helper_.swap(helper);
    LOG(INFO) << "Taking another step for " << cid_;
    Step(listener);
  }
}

void Self::GetHelper(Block::Type typ) {
  LOG(INFO) << "Encountered " << cid_ << " of type " << ipfs::Stringify(typ);
  helper_ = unix_fs::NodeHelper::FromBlockType(typ, pop_path());
  if (helper_) {
    helper_->cid(cid_);
    helper_->resolver(*this);
    helper_->storage(storage_);
    helper_->api(*api_);
  }
}

void Self::Request(std::shared_ptr<DagListener>& listener,
                   std::string const& cid,
                   Priority prio) {
  VLOG(1) << "Request(" << cid << ',' << prio << ')';
  if (storage_.Get(cid)) {
    return;
  }
  auto it = already_requested_.find(cid);
  auto t = std::time(nullptr);
  if (it == already_requested_.end()) {
    VLOG(1) << "Request(" << cid << ',' << static_cast<long>(prio) << ')';
    already_requested_[cid] = {prio, t};
    requestor_.RequestByCid(cid, listener, prio);
    if (prio) {
      storage_.AddListening(this);
      involved_cids_.push_back(cid);
    }
  } else if (prio > it->second.prio) {
    LOG(INFO) << "Increase Request priority(" << cid << ','
              << static_cast<long>(prio) << ')';
    it->second.prio = prio;
    it->second.when = t;
    requestor_.RequestByCid(cid, listener, prio);
  }
}

Self::UnixFsPathResolver(BlockStorage& store,
                         BlockRequestor& requestor,
                         std::string cid,
                         std::string path,
                         std::shared_ptr<ContextApi> api)
    : storage_{store},
      requestor_{requestor},
      cid_{cid},
      path_{path},
      original_path_(path),
      api_(api) {
  LOG(INFO) << "UnixFsPathResolver(storage@" << (void*)(&store) << ')';
  constexpr std::string_view filename_param{"filename="};
  auto fn = original_path_.find(filename_param);
  if (fn != std::string::npos) {
    original_path_.erase(0, fn + filename_param.size());
    auto amp = original_path_.find('&');
    if (amp != std::string::npos) {
      original_path_.erase(amp);
    }
  }
}
Self::~UnixFsPathResolver() noexcept {
  storage_.StopListening(this);
}
std::shared_ptr<ipfs::DagListener> Self::MaybeGetPreviousListener() {
  return old_listener_.lock();
}
std::string const& Self::current_cid() const {
  return cid_;
}
std::string const& Self::original_path() const {
  return original_path_;
}
std::string Self::pop_path() {
  while (path_.size() && path_.front() == '/') {
    path_.erase(0, 1);
  }
  auto slash = path_.find('/');
  std::string rv;
  if (slash > path_.size()) {
    rv = path_;
    path_.clear();
  } else {
    rv = path_.substr(0, slash);
    path_.erase(0, slash + 1);
  }
  return api_->UnescapeUrlComponent(rv);
}
