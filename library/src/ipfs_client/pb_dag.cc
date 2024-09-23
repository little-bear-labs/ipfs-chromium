#include "ipfs_client/pb_dag.h"

#include <ipfs_client/client.h>

#include "log_macros.h"

#include <algorithm>

#if __has_include(<third_party/ipfs_client/pb_dag.pb.h>)
#include <third_party/ipfs_client/pb_dag.pb.h>
#include <third_party/ipfs_client/unix_fs.pb.h>
#else
#include "ipfs_client/pb_dag.pb.h"
#include "ipfs_client/unix_fs.pb.h"
#endif

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

/*! The parsed protobufs
 */
struct ipfs::PbDag::Data {
  pb_dag::PBNode node_;
  unix_fs::Data fsdata_;
};

ipfs::PbDag::PbDag(Cid const& c, std::istream& s)
    : pimpl_{std::make_unique<Data>()}, cid_(c) {
  std::tie(valid_, fs_node_) =
      InitBlock(c.codec(), s, pimpl_->node_, pimpl_->fsdata_);
}

ipfs::PbDag::PbDag(Cid const& c, ByteView s)
    : pimpl_{std::make_unique<Data>()},
      cid_(c),
      original_bytes_(reinterpret_cast<char const*>(s.data()), s.size()) {
  std::tie(valid_, fs_node_) =
      InitBlock(c.codec(), original_bytes_, pimpl_->node_, pimpl_->fsdata_);
}
ipfs::PbDag::PbDag(Cid const& c, std::string_view s) : PbDag(c, as_bytes(s)) {}

ipfs::PbDag::PbDag(PbDag const& rhs)
    : pimpl_(std::make_unique<Data>(*rhs.pimpl_)),
      valid_(rhs.valid_),
      fs_node_(rhs.fs_node_),
      mime_(rhs.mime_),
      cid_(rhs.cid_),
      original_bytes_(rhs.original_bytes_) {}

ipfs::PbDag::PbDag() = default;

ipfs::PbDag::~PbDag() noexcept = default;

bool ipfs::PbDag::valid() const {
  return valid_;
}

auto ipfs::PbDag::type() const -> Type {
  if (!valid()) {
    return Type::Invalid;
  }
  if (!fs_node_) {
    return Type::NonFs;
  }
  if (is_file()) {
    if (pimpl_->fsdata_.blocksizes_size() > 0) {
      return Type::File;
    } else {
      return Type::FileChunk;
    }
  }
  if (pimpl_->fsdata_.type()) {
    return static_cast<Type>(pimpl_->fsdata_.type());
  }
  return Type::Invalid;
}

bool ipfs::PbDag::is_file() const {
  return valid() && fs_node_ &&
         pimpl_->fsdata_.type() == unix_fs::Data_DataType_File;
}

std::string const& ipfs::PbDag::chunk_data() const {
  return pimpl_->fsdata_.data();
}

std::string const& ipfs::PbDag::unparsed() const {
  return pimpl_->node_.data();
}

auto ipfs::PbDag::cid() const -> Cid const& {
  DCHECK(cid_.has_value());
  return cid_.value();
}

std::string ipfs::PbDag::LinkCid(ipfs::ByteView binary_link_hash) const {
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

bool ipfs::PbDag::cid_matches_data(Client& api) const {
  if (!cid_) {
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

std::vector<ipfs::Byte> ipfs::PbDag::binary_hash(Client& api,
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

void ipfs::PbDag::List(
    std::function<bool(std::string const&, std::string)> foo) const {
  for (auto& link : pimpl_->node_.links()) {
    // protobuf uses string for binary data, too
    auto hash = as_bytes(link.hash());
    if (!foo(link.name(), LinkCid(hash))) {
      break;
    }
  }
}
std::optional<std::uint64_t> ipfs::PbDag::Fanout() const {
  if (pimpl_ && fs_node_ && pimpl_->fsdata_.has_fanout()) {
    return pimpl_->fsdata_.fanout();
  }
  return std::nullopt;
}

std::ostream& operator<<(std::ostream& s, ipfs::PbDag::Type t) {
  switch (t) {
    case ipfs::PbDag::Type::Raw:
      return s << "Raw";
    case ipfs::PbDag::Type::Directory:
      return s << "Directory";
    case ipfs::PbDag::Type::File:
      return s << "File";
    case ipfs::PbDag::Type::Metadata:
      return s << "Metadata";
    case ipfs::PbDag::Type::Symlink:
      return s << "Symlink";
    case ipfs::PbDag::Type::HAMTShard:
      return s << "HAMTShard";
    case ipfs::PbDag::Type::FileChunk:
      return s << "FileChunk";
    case ipfs::PbDag::Type::NonFs:
      return s << "NonFs";
    case ipfs::PbDag::Type::Invalid:
      return s << "Invalid";
    default:
      return s << "Invalid value for Block::Type enum (" << static_cast<long>(t)
               << ')';
  }
}
