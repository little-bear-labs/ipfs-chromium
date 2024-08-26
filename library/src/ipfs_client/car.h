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
  Car(ByteView, Client&);
  struct Block {
    Cid cid;
    ByteView bytes;
  };
  std::optional<Block> NextBlock();

 private:
  ByteView data_;
};
}  // namespace ipfs

#endif  // IPFS_CAR_H_
