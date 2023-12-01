#include <ipfs_client/ipld/dag_node.h>

#include "chunk.h"
#include "dag_cbor_node.h"
#include "dag_json_node.h"
#include "directory_shard.h"
#include "ipns_name.h"
#include "root.h"
#include "small_directory.h"
#include "symlink.h"
#include "unixfs_file.h"

#include <ipfs_client/context_api.h>
#include <ipfs_client/dag_block.h>
#include <ipfs_client/ipns_record.h>
#include <libp2p/multi/content_identifier_codec.hpp>

#include "log_macros.h"

using Node = ipfs::ipld::DagNode;

std::shared_ptr<Node> Node::fromBytes(std::shared_ptr<ContextApi> const& api,
                                      Cid const& cid,
                                      std::string_view bytes) {
  using Codec = libp2p::multi::MulticodecType::Code;
  std::shared_ptr<Node> result = nullptr;

  switch (cid.content_type) {
    case Codec::DAG_CBOR: {
      auto p = reinterpret_cast<Byte const*>(bytes.data());
      auto cbor = api->ParseCbor({p, bytes.size()});
      if (cbor) {
        result = std::make_shared<DagCborNode>(std::move(cbor));
      } else {
        LOG(ERROR)
            << "CBOR node "
            << libp2p::multi::ContentIdentifierCodec::toString(cid).value()
            << " does not parse as CBOR.";
      }
    } break;
    case Codec::DAG_JSON: {
      auto json = api->ParseJson(bytes);
      if (json) {
        result = std::make_shared<DagJsonNode>(std::move(json));
      } else {
        LOG(ERROR)
            << "JSON node "
            << libp2p::multi::ContentIdentifierCodec::toString(cid).value()
            << " does not parse as JSON.";
      }
    } break;
    case Codec::RAW:
    case Codec::DAG_PB: {
      ipfs::Block b{cid, bytes};
      if (b.valid()) {
        result = fromBlock(b);
      } else {
        std::ostringstream hex;
        for (auto byt : bytes) {
          hex << ' ' << std::hex
              << static_cast<short>(static_cast<std::uint8_t>(byt));
        }
        LOG(ERROR)
            << "Have a response that did not parse as a valid block, cid: "
            << libp2p::multi::ContentIdentifierCodec::toString(cid).value()
            << " contents: " << bytes.size() << " bytes = " << hex.str();
      }
    } break;
    default:
      LOG(ERROR) << "Response for unhandled CID Codec: "
                 << libp2p::multi::MulticodecType::getName(cid.content_type);
  }
  if (result) {
    result->set_api(api);
  }
  return result;
}
std::shared_ptr<Node> Node::fromBlock(ipfs::Block const& block) {
  std::shared_ptr<Node> result;
  switch (block.type()) {
    case Block::Type::FileChunk:
      return std::make_shared<Chunk>(block.chunk_data());
    case Block::Type::NonFs:
      return std::make_shared<Chunk>(block.unparsed());
    case Block::Type::Symlink:
      return std::make_shared<Symlink>(block.chunk_data());
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

std::ostream& operator<<(std::ostream& s, ipfs::ipld::PathChange const& c) {
  return s << "PathChange{" << c.new_path << '}';
}
