#include "ipfs_client/block.h"

#include <vocab/log_macros.h>

#include <libp2p/multi/multibase_codec/codecs/base32.hpp>
#include <libp2p/multi/multibase_codec/codecs/base58.hpp>

using MC = libp2p::multi::MulticodecType;

namespace {
std::string read_stream(std::istream& is) {
  return std::string(std::istreambuf_iterator<char>(is), {});
}
}  // namespace

ipfs::Block::Block(Multicodec c, std::istream& s) {
  switch (c) {
    case Multicodec::DAG_PB:
      valid_ = node_.ParseFromIstream(&s);
      if (valid_) {
        fs_node_ = fsdata_.ParseFromString(node_.data());
      }
      break;
    case Multicodec::RAW:
      InitFromRaw(read_stream(s));
      break;
    default:
      L_DIE("Stream-initialization unsupported for multicodec: "
            << static_cast<unsigned>(c) << '(' << MC::getName(c) << ')');
  }
}
ipfs::Block::Block(Multicodec c, std::string const& s) {
  switch (c) {
    case Multicodec::DAG_PB:
      valid_ = node_.ParseFromString(s);
      if (valid_) {
        fs_node_ = fsdata_.ParseFromString(node_.data());
      }
      break;
    case Multicodec::RAW:
      InitFromRaw(s);
      break;
    default:
      L_DIE("Unsupported multicodec: " << static_cast<unsigned>(c) << '('
                                       << MC::getName(c) << ')');
  }
}
ipfs::Block::Block(Block const& rhs)
    : node_(rhs.node_),
      fsdata_(rhs.fsdata_),
      valid_(rhs.valid_),
      fs_node_(rhs.fs_node_),
      mime_(rhs.mime_) {}
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
void ipfs::Block::mime_type(std::string_view val) {
  mime_.assign(val);
}
std::string ipfs::Block::LinkCid(std::string const& raw_hash) {
  // If it's 34 bytes long... leading bytes [0x12, 0x20, ...], it's a
  // CIDv0
  if (raw_hash.size() == 34 && raw_hash[0] == 0x12 && raw_hash[1] == 0x20) {
    std::string withmulti({'\x01', '\x70'});
    withmulti.append(raw_hash);
    return "b" + libp2p::multi::detail::encodeBase32Lower(withmulti);
  } else if (raw_hash[0] == 0x01) {
    // let N be the first varint in cid. This is the CID's version
    // The CID's multicodec is the second varint in cid
    //    L_VAR(static_cast<int>(raw_hash[1]));
    return "b" + libp2p::multi::detail::encodeBase32Lower(raw_hash);
  } else if (raw_hash[0] > 1) {
    L_DIE("We don't support CIDv" << std::dec << static_cast<int>(raw_hash[0]));
  } else {
    L_DIE("Invalid binary CID");
  }
  return "TODO";
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
