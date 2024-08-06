#include <ipfs_client/ipld/dag_node.h>

#include "chunk.h"
#include "dag_cbor_node.h"
#include "dag_json_node.h"
#include "directory_shard.h"
#include "dnslink_name.h"
#include "root.h"
#include "small_directory.h"
#include "symlink.h"
#include "unixfs_file.h"

#include <ipfs_client/client.h>
#include <ipfs_client/ipns_record.h>
#include <ipfs_client/pb_dag.h>

#include "log_macros.h"

#include <algorithm>
#include <sstream>

using Node = ipfs::ipld::DagNode;

std::shared_ptr<Node> Node::fromBytes(std::shared_ptr<Client> const& api,
                                      Cid const& cid,
                                      std::string_view bytes) {
  return fromBytes(api, cid, as_bytes(bytes));
}
auto Node::fromBytes(std::shared_ptr<Client> const& api,
                     ipfs::Cid const& cid,
                     ipfs::ByteView bytes) -> NodePtr {
  std::shared_ptr<Node> result = nullptr;
  auto hash = api->Hash(cid.hash_type(), bytes);
  if (!hash.has_value()) {
    LOG(ERROR) << "Could not hash response for " << cid.to_string();
    return {};
  }
  if (hash.value().size() != cid.hash().size()) {
    return {};
  }
  for (auto i = 0U; i < hash.value().size(); ++i) {
    auto e = cid.hash()[i];
    auto a = hash.value().at(i);
    if (e != a) {
      return {};
    }
  }
  auto required = cid.hash();
  auto calculated = hash.value();
  if (!std::equal(required.begin(), required.end(), calculated.begin(),
                  calculated.end())) {
    LOG(ERROR) << "Hash of response did not match the one in the CID "
               << cid.to_string();
    return {};
  }
  switch (cid.codec()) {
    case MultiCodec::DAG_CBOR: {
      auto p = reinterpret_cast<Byte const*>(bytes.data());
      auto cbor = api->cbor().Parse({p, bytes.size()});
      if (cbor) {
        result = std::make_shared<DagCborNode>(std::move(cbor));
      } else {
        LOG(ERROR) << "CBOR node " << cid.to_string()
                   << " does not parse as CBOR.";
      }
    } break;
    case MultiCodec::DAG_JSON: {
      auto p = reinterpret_cast<char const*>(bytes.data());
      auto json = api->json().Parse({p, bytes.size()});
      if (json) {
        result = std::make_shared<DagJsonNode>(std::move(json));
      } else {
        LOG(ERROR) << "JSON node " << cid.to_string()
                   << " does not parse as JSON.";
      }
    } break;
    case MultiCodec::RAW:
    case MultiCodec::DAG_PB: {
      ipfs::PbDag b{cid, bytes};
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
            << cid.to_string() << " contents: " << bytes.size()
            << " bytes = " << hex.str();
      }
    } break;
    case MultiCodec::INVALID:
    case MultiCodec::IDENTITY:
    case MultiCodec::LIBP2P_KEY:
    default:
      LOG(ERROR) << "Response for unhandled CID Codec: "
                 << GetName(cid.codec());
  }
  if (result) {
    result->set_api(api);
  }
  return result;
}
std::shared_ptr<Node> Node::fromBlock(ipfs::PbDag const& block) {
  std::shared_ptr<Node> result;
  switch (block.type()) {
    case PbDag::Type::FileChunk:
      result = std::make_shared<Chunk>(block.chunk_data());
      break;
    case PbDag::Type::NonFs:
      result = std::make_shared<Chunk>(block.unparsed());
      break;
    case PbDag::Type::Symlink:
      result = std::make_shared<Symlink>(block.chunk_data());
      break;
    case PbDag::Type::Directory:
      result = std::make_shared<SmallDirectory>();
      break;
    case PbDag::Type::File:
    case PbDag::Type::Raw:
      result = std::make_shared<UnixfsFile>();
      break;
    case PbDag::Type::HAMTShard:
      if (auto fanout = block.Fanout()) {
        result = std::make_shared<DirShard>(fanout.value());
      } else {
        result = std::make_shared<DirShard>();
      }
      break;
    case PbDag::Type::Metadata:
      LOG(ERROR) << "Metadata blocks unhandled.";
      return result;
    case PbDag::Type::Invalid:
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

std::shared_ptr<Node> Node::deroot() {
  return shared_from_this();
}
std::shared_ptr<Node> Node::rooted() {
  return std::make_shared<Root>(shared_from_this());
}
void Node::set_api(std::shared_ptr<Client> api) {
  api_ = api;
}
/*
auto Node::resolve(SlashDelimited initial_path, BlockLookup blu)
    -> ResolveResult {
  ResolutionState state{initial_path, ResponseSemantic::Http, blu};
  return Resolve(state);
}
*/
auto Node::Resolve(ResolutionState& params) -> ResolveResult {
  if (source_.cid.size()) {
    params.headers.Add(source_);
  }
  return resolve(params);
}
auto Node::CallChild(ipfs::ipld::ResolutionState& state) -> ResolveResult {
  return CallChild(state, state.NextComponent(api_.get()));
}
auto Node::CallChild(ipfs::ipld::ResolutionState& state,
                     std::string_view link_key,
                     std::string_view block_key) -> ResolveResult {
  auto child = FindChild(link_key);
  if (!child) {
    links_.emplace_back(link_key, Link{std::string{block_key}, {}});
  }
  return CallChild(state, link_key);
}
auto Node::CallChild(ResolutionState& state, std::string_view link_key)
    -> ResolveResult {
  auto* child = FindChild(link_key);
  if (!child) {
    return ProvenAbsent{};
  }
  auto& node = child->node;
  if (!node) {
    node = state.GetBlock(child->cid);
  }
  if (node) {
    state.Descend();
    return node->Resolve(state);
  } else {
    std::string needed{"/ipfs/"};
    needed.append(child->cid);
    auto more = state.PathToResolve().to_view();
    if (more.size()) {
      if (more.front() != '/') {
        needed.push_back('/');
      }
      needed.append(more);
    }
    return MoreDataNeeded{needed};
  }
}
auto Node::CallChild(ResolutionState& state,
                     std::function<NodePtr(std::string_view)> gen_child)
    -> ResolveResult {
  auto link_key = state.NextComponent(api_.get());
  auto child = FindChild(link_key);
  if (!child) {
    links_.emplace_back(link_key, Link{{}, {}});
    child = &links_.back().second;
  }
  auto& node = child->node;
  if (!node) {
    node = gen_child(link_key);
    if (!node) {
      return ProvenAbsent{};
    }
  }
  state.Descend();
  return node->Resolve(state);
}
auto Node::FindChild(std::string_view link_key) -> Link* {
  for (auto& [name, link] : links_) {
    if (name == link_key) {
      return &link;
    }
  }
  return nullptr;
}
bool Node::expired() const {
  return false;
}
bool Node::PreferOver(Node const&) const {
  return false;
}

std::ostream& operator<<(std::ostream& s, ipfs::ipld::PathChange const& c) {
  return s << "PathChange{" << c.new_path << '}';
}
