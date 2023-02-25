#ifndef IPFS_BLOCK_H_
#define IPFS_BLOCK_H_

#if __has_include("components/ipfs/pb_dag.pb.h")
#include "components/ipfs/pb_dag.pb.h"   //inside Chromium build
#include "components/ipfs/unix_fs.pb.h"  //inside Chromium build
#else
#include "pb_dag.pb.h"
#include "unix_fs.pb.h"
#endif

#include <libp2p/multi/multicodec_type.hpp>

#include <iosfwd>

namespace ipfs {

class Block {
 public:
  using Multicodec = libp2p::multi::MulticodecType::Code;
  Block(Multicodec, std::istream&);
  Block(Multicodec, std::string const& binary_data);
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
  void mime_type(std::string_view);
  std::string const& mime_type() const;

  template <class Functor>
  void List(Functor foo) const {
    for (auto& link : node_.links()) {
      if (!foo(link.name(), LinkCid(link.hash()))) {
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

  static std::string LinkCid(std::string const&);
  void InitFromRaw(std::string const& content_bytes);
};

}  // namespace ipfs

std::ostream& operator<<(std::ostream&, ipfs::Block::Type);

#endif  // IPFS_BLOCK_H_
