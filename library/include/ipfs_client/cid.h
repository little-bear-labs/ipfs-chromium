#ifndef IPFS_CID_H_
#define IPFS_CID_H_

#include "multi_hash.h"
#include "multicodec.h"

#include <vocab/byte_view.h>

#include <string>
#include <string_view>

namespace ipfs {
/*! A Content ID - a way to identify content based on the content itself
 *    generally via hash
 */
class Cid {
  MultiCodec codec_ = MultiCodec::INVALID;
  MultiHash hash_;

 public:
  Cid() = default;
  Cid(MultiCodec, MultiHash);
  explicit Cid(std::string_view);
  explicit Cid(ByteView);
  bool ReadStart(ByteView&);

  bool valid() const;
  MultiCodec codec() const { return codec_; }
  MultiHash const& multi_hash() const { return hash_; }
  ByteView hash() const;
  HashType hash_type() const;

  std::string to_string() const;
  bool operator==(Cid const&) const = default;

  constexpr static std::size_t MinSerializedLength =
      1 /*cid version*/ + 1 /*codec*/ + 1 /*hash type*/ +
      1 /*hash len, could be zero*/;
};
}  // namespace ipfs

#endif  // IPFS_CID_H_
