#include "ipfs_client/pb_dag.h"

#include <ipfs_client/client.h>

#include "ipfs_client/multicodec.h"
#include "ipfs_client/cid.h"
#include "ipfs_client/multi_hash.h"
#include "log_macros.h"
#include "vocab/byte_view.h"
#include "vocab/byte.h"

#include <algorithm>
#include <string>
#include <istream>
#include <iterator>
#include <utility>
#include <memory>
#include <tuple>
#include <string_view>
#include <vector>
#include <functional>
#include <optional>
#include <cstdint>
#include <ostream>

#if __has_include(<third_party/ipfs_client/pb_dag.pb.h>)
#include <third_party/ipfs_client/pb_dag.pb.h>
#include <third_party/ipfs_client/unix_fs.pb.h>
#else
#include "ipfs_client/pb_dag.pb.h"
#include "ipfs_client/unix_fs.pb.h"
#endif

namespace {
auto get_bytes(std::string const& already_string) -> std::string {
  return already_string;
}

auto get_bytes(std::istream& strm) -> std::string {
  return {std::istreambuf_iterator<char>(strm), {}};
}

auto parse(ipfs::pb_dag::PBNode& n, std::istream& strm) -> bool {
  return n.ParseFromIstream(&strm);
}

auto parse(ipfs::pb_dag::PBNode& n, std::string const& str) -> bool {
  return n.ParseFromString(str);
}

template <class From>
auto InitBlock(ipfs::MultiCodec c,
                                From& from,
                                ipfs::pb_dag::PBNode& n,
                                ipfs::unix_fs::Data& d) -> std::pair<bool, bool> {
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

ipfs::PbDag::PbDag(Cid const& cid, std::istream& stream)
    : pimpl_{std::make_unique<Data>()}, cid_(cid) {
  std::tie(valid_, fs_node_) =
      InitBlock(cid.codec(), stream, pimpl_->node_, pimpl_->fsdata_);
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

auto ipfs::PbDag::valid() const -> bool {
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
  if (pimpl_->fsdata_.type() != 0) {
    return static_cast<Type>(pimpl_->fsdata_.type());
  }
  return Type::Invalid;
}

auto ipfs::PbDag::is_file() const -> bool {
  return valid() && fs_node_ &&
         pimpl_->fsdata_.type() == unix_fs::Data_DataType_File;
}

auto ipfs::PbDag::chunk_data() const -> std::string const& {
  return pimpl_->fsdata_.data();
}

auto ipfs::PbDag::unparsed() const -> std::string const& {
  return pimpl_->node_.data();
}

auto ipfs::PbDag::cid() const -> Cid const& {
  DCHECK(cid_.has_value());
  return cid_.value();
}

auto ipfs::PbDag::LinkCid(ipfs::ByteView binary_link_hash) const -> std::string {
  Cid const result(binary_link_hash);
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

auto ipfs::PbDag::cid_matches_data(Client& api) const -> bool {
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

auto ipfs::PbDag::binary_hash(Client& api,
                                                 HashType algo) const -> std::vector<ipfs::Byte> {
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
  for (const auto& link : pimpl_->node_.links()) {
    // protobuf uses string for binary data, too
    auto hash = as_bytes(link.hash());
    if (!foo(link.name(), LinkCid(hash))) {
      break;
    }
  }
}
auto ipfs::PbDag::Fanout() const -> std::optional<std::uint64_t> {
  if (pimpl_ && fs_node_ && pimpl_->fsdata_.has_fanout()) {
    return pimpl_->fsdata_.fanout();
  }
  return std::nullopt;
}

auto operator<<(std::ostream& s, ipfs::PbDag::Type t) -> std::ostream& {
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
