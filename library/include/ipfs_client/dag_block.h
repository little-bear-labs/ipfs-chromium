#ifndef IPFS_DAG_BLOCK_H_
#define IPFS_DAG_BLOCK_H_

#include "components/ipfs/pb_dag.pb.h"
#include "components/ipfs/unix_fs.pb.h"

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

  Block(Block const&);
  Block& operator=(Block const&) = default;

  Block();  // Make an invalid block

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
  bool valid_ = false;
  bool fs_node_ = false;
  std::string mime_ = {};
  std::optional<Cid> cid_ = std::nullopt;
  std::string original_bytes_;

  std::string LinkCid(ipfs::ByteView) const;

  void InitFromRaw(std::string const& content_bytes);
};

}  // namespace ipfs

std::ostream& operator<<(std::ostream&, ipfs::Block::Type);

#endif  // IPFS_DAG_BLOCK_H_
