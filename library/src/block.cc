#include "ipfs_client/block.h"

#include <libp2p/multi/multibase_codec/codecs/base32.hpp>
#include <libp2p/multi/multibase_codec/codecs/base58.hpp>

ipfs::Block::Block(std::istream& s) {
  valid_ = node_.ParseFromIstream(&s);
  if (valid_) {
    fs_node_ = fsdata_.ParseFromString(node_.data());
  }
}
ipfs::Block::Block(std::string const& s) {
  valid_ = node_.ParseFromString(s);
  if (valid_) {
    fs_node_ = fsdata_.ParseFromString(node_.data());
  }
}
ipfs::Block::Block(Block const& rhs)
    : node_(rhs.node_),
      fsdata_(rhs.fsdata_),
      valid_(rhs.valid_),
      fs_node_(rhs.fs_node_),
      mime_(rhs.mime_) {}

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
void ipfs::Block::mime_type(std::string_view val) {
  mime_.assign(val);
}
std::string ipfs::Block::LinkCid(std::string const& raw_hash) {
  std::string withmulti({'\x01', '\x70'});
  withmulti.append(raw_hash);
  //  return libp2p::multi::detail::encodeBase58(raw_hash);
  return "b" + libp2p::multi::detail::encodeBase32Lower(withmulti);
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
