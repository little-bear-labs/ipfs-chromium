#include "multi_node_file.h"

#include "guess_content_type.h"

#include <ipfs_client/block_storage.h>
#include <ipfs_client/dag_block.h>
#include <ipfs_client/unixfs_path_resolver.h>

#include <vocab/stringify.h>

#include "log_macros.h"

using Self = ipfs::unix_fs::MultiNodeFile;

bool Self::Process(std::unique_ptr<NodeHelper>&,
                   std::shared_ptr<DagListener> listener,
                   std::function<void(std::string, Priority)> requestor,
                   std::string& target_cid) {
  if (!block()) {
    requestor(cid_, resolver_->priority());
    return false;
  }
  Fetch(requestor);
  if (Write(listener)) {
    listener->BlocksComplete(
        GuessContentType(*api_, resolver_->original_path(), FirstChunk()));
    target_cid.clear();
    return true;
  }
  return false;
}

void Self::Fetch(std::function<void(std::string, Priority)> requestor) {
  auto idx = -1L;
  block()->List([this, requestor, &idx](auto&, auto cid) {
    ++idx;
    if (idx >= static_cast<long>(children_.size())) {
      children_.emplace_back(cid, std::nullopt);
    }
    auto child_block = storage_->Get(cid);
    if (!child_block) {
      requestor(cid, resolver_->priority());
      return true;
    }
    if (child_block->type() != Block::Type::File) {
      return true;
    }
    auto& child = children_[idx].second;
    if (!child.has_value()) {
      child = MultiNodeFile();
      Delegate(*child);
      child->cid(cid);
    }
    child->Fetch(requestor);
    return true;
  });
}
bool Self::Write(std::shared_ptr<DagListener> listener) {
  LOG(INFO) << "Write:" << cid_ << " have children: " << children_.size();
  for (; written_until_ < children_.size(); ++written_until_) {
    auto& child = children_[written_until_];
    //    LOG(INFO) << "child[" << written_until_ << "]:" << child.first;
    if (child.second.has_value()) {
      if (child.second->Write(listener)) {
        LOG(INFO) << "Successfully recursed.";
        continue;
      } else {
        return false;
      }
    }
    auto* child_block = storage_->Get(child.first);
    if (!child_block) {
      return false;
    }
    switch (child_block->type()) {
      case Block::Type::FileChunk:
        listener->ReceiveBlockBytes(child_block->chunk_data());
        break;
      case Block::Type::NonFs:
        listener->ReceiveBlockBytes(child_block->unparsed());
        break;
      case Block::Type::File:
        LOG(ERROR) << "Child helper should already be populated.";
        return false;
      default:
        LOG(ERROR) << "Member of a multi-node file of unhandled type: "
                   << Stringify(child_block->type());
        return false;
    }
  }
  DCHECK_EQ(written_until_, children_.size());
  return true;
}
std::string Self::FirstChunk() const {
  if (children_.empty()) {
    return {};
  }
  auto& child = children_.front();
  if (child.second.has_value()) {
    return child.second->FirstChunk();
  }
  auto* first = storage_->Get(child.first);
  if (!first) {
    return {};
  }
  switch (first->type()) {
    case Block::Type::FileChunk:
      return first->chunk_data();
    case Block::Type::NonFs:
      return first->unparsed();
    case Block::Type::File:
      LOG(ERROR) << "Child helper should already be populated.";
      return {};
    default:
      LOG(ERROR) << "Member of a multi-node file of unhandled type: "
                 << Stringify(first->type());
      return {};
  }
}

Self::MultiNodeFile() {}
Self::MultiNodeFile(MultiNodeFile const& rhs) = default;
Self::~MultiNodeFile() noexcept {}
