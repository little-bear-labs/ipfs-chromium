#include <ipfs_client/ipld/dag_node.h>

#include "chunk.h"
#include "directory_shard.h"
#include "ipns_name.h"
#include "root.h"
#include "small_directory.h"
#include "unixfs_file.h"

#include <ipfs_client/dag_block.h>
#include <ipfs_client/ipns_record.h>

#include "log_macros.h"

using Node = ipfs::ipld::DagNode;

std::shared_ptr<Node> Node::fromBlock(ipfs::Block const& block) {
  std::shared_ptr<Node> result;
  switch (block.type()) {
    case Block::Type::FileChunk:
      return std::make_shared<Chunk>(block.chunk_data());
    case Block::Type::NonFs:
      return std::make_shared<Chunk>(block.unparsed());
    case Block::Type::Directory:
      result = std::make_shared<SmallDirectory>();
      break;
    case Block::Type::File:
      result = std::make_shared<UnixfsFile>();
      break;
    case Block::Type::HAMTShard:
      if (block.fsdata().has_fanout()) {
        result = std::make_shared<DirShard>(block.fsdata().fanout());
      } else {
        result = std::make_shared<DirShard>();
      }
      break;
    case Block::Type::Invalid:
      LOG(ERROR) << "Invalid block.";
      return result;
    default:
      LOG(FATAL) << "TODO " << static_cast<long>(block.type());
  }
  auto add_link = [&result](auto& n, auto c) {
    result->links_.emplace_back(n, c);
    return true;
  };
  block.List(add_link);
  return result;
}

auto Node::fromIpnsRecord(ipfs::ValidatedIpns const& v) -> NodePtr {
  return std::make_shared<IpnsName>(v.value);
}

std::shared_ptr<Node> Node::deroot() {
  return shared_from_this();
}
std::shared_ptr<Node> Node::rooted() {
  return std::make_shared<Root>(shared_from_this());
}
auto Node::as_hamt() -> DirShard* {
  return nullptr;
}
void Node::set_api(std::shared_ptr<ContextApi> api) {
  api_ = api;
}