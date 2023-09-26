#ifndef IPFS_DAG_BLOCK_H_
#define IPFS_DAG_BLOCK_H_

#if __has_include(<third_party/ipfs_client/pb_dag.pb.h>)
#include <third_party/ipfs_client/pb_dag.pb.h>
#include <third_party/ipfs_client/unix_fs.pb.h>
#else
#include "ipfs_client/pb_dag.pb.h"
#include "ipfs_client/unix_fs.pb.h"
#endif

#include <libp2p/multi/content_identifier.hpp>
#include <libp2p/multi/hash_type.hpp>
#include <libp2p/multi/multicodec_type.hpp>

#include <vocab/byte_view.h>

#include <iosfwd>
#include <optional>

namespace ipfs {

using Cid = libp2p::multi::ContentIdentifier;

/*!
 * \brief Something to which a CID may refer directly.
 * \details A block may be "raw" - just a bunch of bytes.
 *    Or it could be an UnixFS-encoded node in a DAG
 *    Or it could be something else, like DAG-CBOR
 *    But this class really only handles the first 2 so far.
 */
class Block {
 public:
  using Multicodec = libp2p::multi::MulticodecType::Code;

  /*!
   * \brief Initialize from stream
   * \param cid    - The Content IDentifier
   * \param stream - Stream from which one can read the bytes of the block
   */
  Block(Cid const& cid, std::istream& stream);

  /*!
   * \brief Initialize from block of bytes
   * \param cid   - The Content IDentifier
   * \param bytes - The bytes to be interpreted as a maybe-node
   * \note It's not really a string - certainly not text in any way.
   *    It's just a container of arbitrary bytes.
   */
  Block(Cid const& cid, std::string const& bytes);

  Block(Block const&);
  Block& operator=(Block const&) = default;

  Block();  ///< Construct an invalid block

  ~Block() noexcept;

  bool valid() const;  ///< Check if the block appears valid

  /*!
   * \brief The kinds of things a block may be representing
   */
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

  Type type() const;  ///< Accessor for this block's type

  bool is_file() const;  ///< type() == File || type() == FileChunk

  bool is_directory() const;  ///< type() == Directory

  /*!
   * \brief Get file size from the UnixFS node data
   * \return zero if such data is not available, e.g. for raw or directory
   */
  std::uint64_t file_size() const;

  std::string const& chunk_data() const;  ///< data field from a UnixFS node

  std::string const& unparsed() const;  ///< Original bytes (with protobuf bits)

  /*!
   * \brief Accessor for all UnixFS data as a protobuf object
   * \deprecated
   */
  unix_fs::Data const& fsdata() const { return fsdata_; }

  Cid const& cid() const;  ///< Getter for Content IDentifier

  void mime_type(std::string_view);  ///< mime type setter - not managed

  std::string const& mime_type() const;  ///< Getter for mime type

  bool cid_matches_data() const;  ///< Basic validation

  std::vector<Byte> binary_hash(libp2p::multi::HashType) const;

  /*!
   * \brief Iterate through the links of this UnixFS node
   * \param foo - Called for each link with (name, cid)
   *    should return converts-to-bool
   *    name is convertable from std::string const&
   *    cid  is convertable from std::string&&
   */
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
