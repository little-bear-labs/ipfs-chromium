#ifndef IPFS_PB_DAG_H_
#define IPFS_PB_DAG_H_

#if __has_include(<third_party/ipfs_client/pb_dag.pb.h>)
#include <third_party/ipfs_client/pb_dag.pb.h>
#include <third_party/ipfs_client/unix_fs.pb.h>
#else
#include "ipfs_client/pb_dag.pb.h"
#include "ipfs_client/unix_fs.pb.h"
#endif

#include "cid.h"

#include <libp2p/multi/multicodec_type.hpp>

#include <vocab/byte_view.h>

#include <iosfwd>
#include <optional>

namespace ipfs {

class ContextApi;

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
  PbDag(Cid const& cid, std::string_view bytes);

  PbDag(PbDag const&);
  PbDag& operator=(PbDag const&) = default;

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

  /*!
   * \brief Accessor for all UnixFS data as a protobuf object
   * \deprecated
   */
  unix_fs::Data const& fsdata() const { return fsdata_; }

  Cid const& cid() const;  ///< Getter for Content IDentifier

  bool cid_matches_data(ContextApi&) const;  ///< Basic validation

  /// Pass INVALID to mean cid().hash_type()
  std::vector<Byte> binary_hash(ContextApi&,
                                HashType = HashType::INVALID) const;

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

};

}  // namespace ipfs

std::ostream& operator<<(std::ostream&, ipfs::PbDag::Type);

#endif  // IPFS_PB_DAG_H_
