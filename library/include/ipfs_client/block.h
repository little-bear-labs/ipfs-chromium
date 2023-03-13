#ifndef IPFS_BLOCK_H_
#define IPFS_BLOCK_H_

#if __has_include("components/ipfs/pb_dag.pb.h")
// inside Chromium build
#include "components/ipfs/pb_dag.pb.h"
#include "components/ipfs/unix_fs.pb.h"

#elif __has_include("library/pb_dag.pb.h")
#include "library/pb_dag.pb.h"
#include "library/unix_fs.pb.h"
#else
#include "pb_dag.pb.h"
#include "unix_fs.pb.h"
#endif

#include <libp2p/multi/content_identifier.hpp>
#include <libp2p/multi/hash_type.hpp>
#include <libp2p/multi/multicodec_type.hpp>

#include <vocab/byte_view.h>

#include <iosfwd>
#include <optional>

namespace ipfs {

using Cid = libp2p::multi::ContentIdentifier;

class Block {
 public:
  using Multicodec = libp2p::multi::MulticodecType::Code;

  Block(Cid const&, std::istream&);

  Block(Cid const&, std::string const&);

  // TODO remove these or at least make them private
  //  Block(Multicodec, std::istream&);

  //  Block(Multicodec, std::string const& binary_data);

  Block(Block const&);

  ~Block() noexcept;

  bool valid() const;

  enum class Type {
    Raw,
    Directory,
    File,
    Metadata,
    Symlink,
    HAMTShard,
    FileChunk,
    NonFs,
    Invalid,
  };

  Type type() const;

  bool is_file() const;

  bool is_directory() const;

  std::uint64_t file_size() const;

  std::string const& chunk_data() const;

  std::string const& unparsed() const;

  unix_fs::Data const& fsdata() const { return fsdata_; }

  Cid const& cid() const;

  void mime_type(std::string_view);

  std::string const& mime_type() const;

  bool cid_matches_data() const;

  std::basic_string<Byte> binary_hash(libp2p::multi::HashType) const;

  template <class Functor>
  void List(Functor foo) const {
    for (auto& link : node_.links()) {
      // protobuf uses string for binary data, too
      auto hash = ipfs::ByteView{
          reinterpret_cast<ipfs::Byte const*>(link.hash().data()),
          link.hash().size()};
      if (!foo(link.name(), LinkCid(hash))) {
        break;
      }
    }
  }

 private:
  pb_dag::PBNode node_;
  unix_fs::Data fsdata_;
  bool valid_;
  bool fs_node_ = false;
  std::string mime_ = {};
  std::optional<Cid> cid_;
  std::string original_bytes_;

  static std::string LinkCid(ipfs::ByteView);

  void InitFromRaw(std::string const& content_bytes);
};

}  // namespace ipfs

std::ostream& operator<<(std::ostream&, ipfs::Block::Type);

#endif  // IPFS_BLOCK_H_
