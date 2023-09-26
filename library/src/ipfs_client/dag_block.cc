#include "ipfs_client/dag_block.h"

#include <libp2p/crypto/hasher.hpp>
#include <libp2p/multi/content_identifier_codec.hpp>
#include <libp2p/multi/multihash.hpp>

#include "log_macros.h"

#include <algorithm>

#include <algorithm>

using MC = libp2p::multi::MulticodecType;

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

using Multicodec = libp2p::multi::MulticodecType::Code;

template <class From>
std::pair<bool, bool> InitBlock(Multicodec c,
                                From& from,
                                ipfs::pb_dag::PBNode& n,
                                ipfs::unix_fs::Data& d) {
  switch (c) {
    case Multicodec::DAG_PB:
      if (parse(n, from)) {
        return {true, d.ParseFromString(n.data())};
      }
      break;
    case Multicodec::RAW:
    case Multicodec::IDENTITY:
      d.set_type(ipfs::unix_fs::Data_DataType_File);
      d.set_data(get_bytes(from));
      d.set_filesize(d.data().size());
      n.set_data(d.SerializeAsString());
      return {true, true};
    default:
      LOG(FATAL) << "Block-initialization unsupported for multicodec: "
                 << static_cast<unsigned>(c) << '('
                 << std::string{MC::getName(c)} << ')';
  }
  return {false, false};
}
}  // namespace

ipfs::Block::Block(Cid const& c, std::istream& s) : cid_(c) {
  std::tie(valid_, fs_node_) = InitBlock(c.content_type, s, node_, fsdata_);
}

ipfs::Block::Block(Cid const& c, std::string const& s)
    : cid_(c), original_bytes_(s) {
  std::tie(valid_, fs_node_) = InitBlock(c.content_type, s, node_, fsdata_);
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

void ipfs::Block::InitFromRaw(std::string const& content_bytes) {
  fsdata_.set_type(unix_fs::Data_DataType_File);
  fsdata_.set_data(content_bytes);
  fsdata_.set_filesize(content_bytes.size());
  node_.set_data(fsdata_.SerializeAsString());
  valid_ = true;
  fs_node_ = true;
}

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

bool ipfs::Block::is_directory() const {
  return valid() && fs_node_ &&
         fsdata_.type() == unix_fs::Data_DataType_Directory;
}

std::uint64_t ipfs::Block::file_size() const {
  if (fs_node_ && fsdata_.has_filesize()) {
    return fsdata_.filesize();
  } else {
    return 0UL;
  }
}

std::string const& ipfs::Block::chunk_data() const {
  return fsdata_.data();
}

std::string const& ipfs::Block::unparsed() const {
  return node_.data();
}

std::string const& ipfs::Block::mime_type() const {
  return mime_;
}
auto ipfs::Block::cid() const -> Cid const& {
  DCHECK(cid_.has_value());
  return cid_.value();
}
void ipfs::Block::mime_type(std::string_view val) {
  mime_.assign(val);
}

std::string ipfs::Block::LinkCid(ipfs::ByteView binary_link_hash) const {
  using Codec = libp2p::multi::ContentIdentifierCodec;
  auto result = Codec::decode(binary_link_hash);
  if (!result.has_value()) {
    LOG(FATAL) << "Failed to decode link CID as binary ( link from "
               << Codec::toString(cid()).value()
               << "): " << Stringify(result.error());
  }
  auto str_res = Codec::toString(result.value());
  if (!str_res.has_value()) {
    LOG(FATAL) << "Failed to decode binary link CID as string (link from "
               << Codec::toString(cid()).value()
               << "): " << Stringify(result.error());
  }
  return str_res.value();
}

bool ipfs::Block::cid_matches_data() const {
  if (!cid_) {
    // TODO - probably remove those constructors and make cid_ not optional
    return true;
  }
  auto cid_hash = cid_->content_address.getHash();
  auto hash_type = cid_->content_address.getType();
  if (hash_type == libp2p::multi::HashType::identity) {
    return true;
  }
  auto hashed = this->binary_hash(hash_type);
  return std::equal(cid_hash.begin(), cid_hash.end(), hashed.begin(),
                    hashed.end());
}

std::vector<ipfs::Byte> ipfs::Block::binary_hash(
    libp2p::multi::HashType algo) const {
  ipfs::ByteView bytes{reinterpret_cast<Byte const*>(original_bytes_.data()),
                       original_bytes_.size()};
  auto hasher = libp2p::crypto::CreateHasher(algo);
  std::vector<ipfs::Byte> result(hasher->digestSize(), Byte{});
  if (hasher->write(bytes).value()) {
    if (!hasher->digestOut({result.data(), result.size()}).has_value()) {
      LOG(ERROR) << "Error getting digest.";
    }
  } else {
    LOG(ERROR) << "Attempt to hash bytes returned false";
  }
  return result;
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
