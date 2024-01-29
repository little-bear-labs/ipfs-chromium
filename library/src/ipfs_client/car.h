#ifndef IPFS_CAR_H_
#define IPFS_CAR_H_

#include <ipfs_client/cid.h>
#include <vocab/byte_view.h>

#include <memory>
#include <optional>

namespace ipfs {
class Client;
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
