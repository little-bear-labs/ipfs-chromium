#ifndef IPFS_CID_H_
#define IPFS_CID_H_

#include "multi_hash.h"
#include "multicodec.h"

#include <vocab/byte_view.h>

#include <string>
#include <string_view>

namespace ipfs {
class Cid {
  MultiCodec codec_ = MultiCodec::INVALID;
  MultiHash hash_;

  void assign(ByteView);

 public:
  Cid(MultiCodec, MultiHash);
  explicit Cid(std::string_view);
  explicit Cid(ByteView);

  bool valid() const;
  MultiCodec codec() const { return codec_; }
  MultiHash const& multi_hash() const { return hash_; }
  ByteView hash() const;
  HashType hash_type() const;

  std::string to_string() const;
};
}  // namespace ipfs

#endif  // IPFS_CID_H_
