#include "ipfs_client/unixfs_path_resolver.h"

#include "unix_fs/node_helper.h"

#include "ipfs_client/block_storage.h"
#include "ipfs_client/dag_block.h"

#include "vocab/stringify.h"

#include "log_macros.h"

using Self = ipfs::UnixFsPathResolver;

void Self::Step(std::shared_ptr<DagListener> listener) {
  old_listener_ = listener;
  if (cid_.empty()) {
    return;
  }
  LOG(INFO) << "Step(" << cid_ << ',' << path_ << ',' << original_path_ << ')';
  Block const* block = storage_.Get(cid_);
  if (!block) {
    LOG(INFO) << "Current block " << cid_ << " not found. Requesting.";
    Request(listener, cid_, prio_);
    return;
  }
  if (!helper_) {
    GetHelper(block->type());
  }
  if (!helper_) {
    listener->FourOhFour(cid_, original_path_);
    return;
  }
  LOG(INFO) << "Processing block " << cid_ << " of type "
            << ipfs::Stringify(block->type());
  auto requestor = [this, &listener](std::string cid, Priority prio) {
    this->Request(listener, cid, prio);
  };
  std::unique_ptr<unix_fs::NodeHelper> helper;
  if (helper_->Process(helper, listener, requestor, cid_)) {
    helper_.swap(helper);
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
  if (storage_.Get(cid)) {
    return;
  }
  if (prio) {
    storage_.AddListening(this);
  }
  auto it = already_requested_.find(cid);
  auto t = std::time(nullptr);
  if (it == already_requested_.end()) {
    LOG(INFO) << "Request(" << cid << ',' << static_cast<long>(prio) << ')';
    already_requested_[cid] = {prio, t};
    api_->RequestByCid(cid, listener, prio);
  } else if (prio > it->second.first) {
    LOG(INFO) << "Increase Request priority(" << cid << ','
              << static_cast<long>(prio) << ')';
    it->second.first = prio;
    it->second.second = t;
    api_->RequestByCid(cid, listener, prio);
  }
}

Self::UnixFsPathResolver(BlockStorage& store,
                         std::string cid,
                         std::string path,
                         std::shared_ptr<NetworkingApi> api)
    : storage_{store}, cid_{cid}, path_{path}, original_path_(path), api_(api) {
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
