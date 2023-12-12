#include "ipfs_client/dag_block.h"

#include <ipfs_client/context_api.h>

#include "log_macros.h"

#include <algorithm>

#include <algorithm>

namespace {
std::string get_bytes(std::string const& s) {
  return s;
}

std::string get_bytes(std::istream& is) {
  return std::string(std::istreambuf_iterator<char>(is), {});
}

bool parse(ipfs::pb_dag::PBNode& n, std::istream& s) {
  return n.ParseFromIstream(&s);
}

bool parse(ipfs::pb_dag::PBNode& n, std::string const& s) {
  return n.ParseFromString(s);
}

template <class From>
std::pair<bool, bool> InitBlock(ipfs::MultiCodec c,
                                From& from,
                                ipfs::pb_dag::PBNode& n,
                                ipfs::unix_fs::Data& d) {
  using Cdc = ipfs::MultiCodec;
  switch (c) {
    case Cdc::DAG_PB:
      if (parse(n, from)) {
        return {true, d.ParseFromString(n.data())};
      }
      break;
    case Cdc::RAW:
    case Cdc::IDENTITY:
      d.set_type(ipfs::unix_fs::Data_DataType_File);
      d.set_data(get_bytes(from));
      d.set_filesize(d.data().size());
      n.set_data(d.SerializeAsString());
      return {true, true};
    case Cdc::INVALID:
    case Cdc::DAG_CBOR:
    case Cdc::LIBP2P_KEY:
    case Cdc::DAG_JSON:
    default:
      LOG(FATAL) << "Block-initialization unsupported for multicodec: "
                 << static_cast<unsigned>(c) << '(' << std::string{GetName(c)}
                 << ')';
  }
  return {false, false};
}
}  // namespace

ipfs::Block::Block(Cid const& c, std::istream& s) : cid_(c) {
  std::tie(valid_, fs_node_) = InitBlock(c.codec(), s, node_, fsdata_);
}

ipfs::Block::Block(Cid const& c, std::string_view s)
    : cid_(c), original_bytes_(s) {
  std::tie(valid_, fs_node_) =
      InitBlock(c.codec(), original_bytes_, node_, fsdata_);
}

ipfs::Block::Block(Block const& rhs)
    : node_(rhs.node_),
      fsdata_(rhs.fsdata_),
      valid_(rhs.valid_),
      fs_node_(rhs.fs_node_),
      mime_(rhs.mime_),
      cid_(rhs.cid_),
      original_bytes_(rhs.original_bytes_) {}

ipfs::Block::Block() = default;

ipfs::Block::~Block() noexcept {}

bool ipfs::Block::valid() const {
  return valid_;
}

auto ipfs::Block::type() const -> Type {
  if (!valid()) {
    return Type::Invalid;
  }
  if (!fs_node_) {
    return Type::NonFs;
  }
  if (is_file()) {
    if (fsdata_.blocksizes_size() > 0) {
      return Type::File;
    } else {
      return Type::FileChunk;
    }
  }
  if (fsdata_.type()) {
    return static_cast<Type>(fsdata_.type());
  }
  return Type::Invalid;
}

bool ipfs::Block::is_file() const {
  return valid() && fs_node_ && fsdata_.type() == unix_fs::Data_DataType_File;
}

std::string const& ipfs::Block::chunk_data() const {
  return fsdata_.data();
}

std::string const& ipfs::Block::unparsed() const {
  return node_.data();
}

auto ipfs::Block::cid() const -> Cid const& {
  DCHECK(cid_.has_value());
  return cid_.value();
}

std::string ipfs::Block::LinkCid(ipfs::ByteView binary_link_hash) const {
  Cid result(binary_link_hash);
  if (!result.valid()) {
    LOG(FATAL) << "Failed to decode link CID as binary ( link from "
               << cid().to_string() << ")";
  }
  auto str_res = result.to_string();
  if (str_res.empty()) {
    LOG(FATAL) << "Failed to decode binary link CID as string (link from "
               << cid().to_string() << ")";
  }
  return str_res;
}

bool ipfs::Block::cid_matches_data(ContextApi& api) const {
  if (!cid_) {
    // TODO - probably remove those constructors and make cid_ not optional
    return true;
  }
  if (type() == Type::Invalid) {
    LOG(ERROR) << "CID can't match data on an invalid block.";
    return false;
  }
  auto cid_hash = cid_->hash();
  auto hash_type = cid_->hash_type();
  if (hash_type == HashType::IDENTITY) {
    return true;
  }
  auto hashed = this->binary_hash(api, hash_type);
  return std::equal(cid_hash.begin(), cid_hash.end(), hashed.begin(),
                    hashed.end());
}

std::vector<ipfs::Byte> ipfs::Block::binary_hash(ContextApi& api,
                                                 HashType algo) const {
  if (algo == HashType::INVALID) {
    algo = cid().hash_type();
  }
  auto result = api.Hash(algo, as_bytes(original_bytes_));
  if (result.has_value()) {
    return result.value();
  } else {
    return {};
  }
}

std::ostream& operator<<(std::ostream& s, ipfs::Block::Type t) {
  switch (t) {
    case ipfs::Block::Type::Raw:
      return s << "Raw";
    case ipfs::Block::Type::Directory:
      return s << "Directory";
    case ipfs::Block::Type::File:
      return s << "File";
    case ipfs::Block::Type::Metadata:
      return s << "Metadata";
    case ipfs::Block::Type::Symlink:
      return s << "Symlink";
    case ipfs::Block::Type::HAMTShard:
      return s << "HAMTShard";
    case ipfs::Block::Type::FileChunk:
      return s << "FileChunk";
    case ipfs::Block::Type::NonFs:
      return s << "NonFs";
    case ipfs::Block::Type::Invalid:
      return s << "Invalid";
    default:
      return s << "Invalid value for Block::Type enum (" << static_cast<long>(t)
               << ')';
  }
}
