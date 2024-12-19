#ifndef IPFS_PB_DAG_H_
#define IPFS_PB_DAG_H_

#include "cid.h"

#include <vocab/byte_view.h>

#include <functional>
#include <iosfwd>
#include <memory>
#include <optional>

namespace ipfs {

class Client;

/*!
 * \brief Something to which a CID may refer directly.
 * \details A block may be "raw" - just a bunch of bytes.
 *    Or it could be an UnixFS-encoded node in a DAG
 *    Or it could be something else, like DAG-CBOR
 *    But this class really only handles the first 2 so far.
 */
class PbDag {
 public:
  /*!
   * \brief Initialize from stream
   * \param cid    - The Content IDentifier
   * \param stream - Stream from which one can read the bytes of the block
   */
  PbDag(Cid const& cid, std::istream& stream);

  /*!
   * \brief Initialize from block of bytes
   * \param cid   - The Content IDentifier
   * \param bytes - The bytes to be interpreted as a maybe-node
   * \note It's not really a string - certainly not text in any way.
   *    It's just a container of arbitrary bytes.
   */
  PbDag(Cid const& cid, ByteView bytes);
  /*! Convenience constructor
   *  @param cid - The CID of the block
   *  @param bytes - The block
   */
  PbDag(Cid const& cid, std::string_view bytes);

  PbDag(PbDag const&);
//  PbDag& operator=(PbDag const&) = default;

  PbDag();  ///< Construct an invalid block

  ~PbDag() noexcept;

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

  std::string const& chunk_data() const;  ///< data field from a UnixFS node

  std::string const& unparsed() const;  ///< Original bytes (with protobuf bits)

  Cid const& cid() const;  ///< Getter for Content IDentifier

  bool cid_matches_data(Client&) const;  ///< Basic validation

  /// Pass INVALID to mean cid().hash_type()
  std::vector<Byte> binary_hash(Client&, HashType = HashType::INVALID) const;

  void List(std::function<bool(std::string const&, std::string)>) const;
  std::optional<std::uint64_t> Fanout() const;

 private:
  struct Data;
  std::unique_ptr<Data> pimpl_;
  bool valid_ = false;
  bool fs_node_ = false;
  std::string mime_ = {};
  std::optional<Cid> cid_ = std::nullopt;
  std::string original_bytes_;

  std::string LinkCid(ipfs::ByteView) const;

};

}  // namespace ipfs

std::ostream& operator<<(std::ostream&, ipfs::PbDag::Type);

#endif  // IPFS_PB_DAG_H_
