#include "node_helper.h"

#include "dir_shard.h"
#include "multi_node_file.h"
#include "plain_directory.h"
#include "plain_file.h"

#include <ipfs_client/block_storage.h>

#include <vocab/stringify.h>

#include "log_macros.h"

using Self = ipfs::unix_fs::NodeHelper;

auto Self::block() -> Block const* {
  return storage_->Get(cid_);
}

void Self::Delegate(ipfs::unix_fs::NodeHelper& other) const {
  L_VAR((void*)this);
  L_VAR((void*)&other);
  other.cid_ = cid_;
  LOG(INFO) << "NodeHelper::Delegate(storage @" << (void*)(storage_.get())
            << ')';
  other.storage_ = storage_;
  other.resolver_ = resolver_;
  other.api_ = api_;
}
void Self::storage(ipfs::BlockStorage& val) {
  LOG(INFO) << "NodeHelper::storage(@" << (void*)(&val) << ')';
  storage_ = &val;
}
auto Self::FromBlockType(Block::Type typ, std::string path_element)
    -> std::unique_ptr<NodeHelper> {
  switch (typ) {
    case Block::Type::Directory:
      return std::make_unique<unix_fs::PlainDirectory>(path_element);
    case Block::Type::FileChunk:
      return std::make_unique<unix_fs::PlainFile>();
    case Block::Type::File:
      return std::make_unique<unix_fs::MultiNodeFile>();
    case Block::Type::HAMTShard:
      return std::make_unique<unix_fs::DirShard>(path_element);
    default:
      LOG(FATAL) << "TODO : Unhandled UnixFS node " << path_element
                 << " of type " << Stringify(typ);
      return {};
  }
}

Self::~NodeHelper() noexcept {}
