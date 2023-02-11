#ifndef IPFS_BLOCK_H_
#define IPFS_BLOCK_H_

#if __has_include("components/ipfs/pb_dag.pb.h")
#include "components/ipfs/pb_dag.pb.h"   //inside Chromium build
#include "components/ipfs/unix_fs.pb.h"  //inside Chromium build
#else
#include "gen/pb_dag.pb.h"
#include "gen/unix_fs.pb.h"
#endif

#include <iosfwd>

namespace ipfs {

class Block {
 public:
  Block(std::istream&);
  explicit Block(std::string const& binary_data);
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

  template <class Functor>
  void List(Functor foo) {
    if (!is_directory()) {
      return;
    }
    for (auto& link : node_.links()) {
      foo(link.name(), LinkCid(link.hash()));
    }
  }

 private:
  pb_dag::PBNode node_;
  unix_fs::Data fsdata_;
  bool valid_;
  bool fs_node_ = false;

  static std::string LinkCid(std::string const&);
};

}  // namespace ipfs

std::ostream& operator<<(std::ostream&, ipfs::Block::Type);

#endif  // IPFS_BLOCK_H_
