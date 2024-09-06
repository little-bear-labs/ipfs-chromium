#ifndef IPFS_CAR_H_
#define IPFS_CAR_H_

#include <ipfs_client/cid.h>
#include <vocab/byte_view.h>

#include <memory>
#include <optional>

namespace ipfs {
class Client;
/*! A (C)ontent-addressed (A)(r)chive
 *  Archive in the same sense as tape archive (.tar), Java archive (.jar), archive (.a) etc.
 *  However contains metadata not related to a filesystem, but to an IPLD DAG (or part of one)
 */
class Car {
 public:
  /*! Parse a CAR file
   *  @param bytes The serialized form of the CAR
   *  @param api   IPFS Client context
   *  @todo Since we're not extending the lifetime, we could take a more specific (raw) ref
   *    to the CBOR parser, since that should be the only part of Client actually used.
   */
  Car(ByteView bytes, Client& api);

  /*! An IPLD block discvered in a CAR file
   */
  struct Block {
    Cid cid;///< The Content ID of the block
    ByteView bytes;///< The raw bytes that get hashed for the CID
  };
  /*! Pop the next block of the archive
   *  @return The block, or nullopt if there are no more
   */
  std::optional<Block> NextBlock();

 private:
  ByteView data_;
};
}  // namespace ipfs

#endif  // IPFS_CAR_H_
