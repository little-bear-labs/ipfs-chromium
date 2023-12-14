#ifndef IPFS_MULTI_HASH_H_
#define IPFS_MULTI_HASH_H_

#include <vocab/byte_view.h>

#include <vector>

namespace ipfs {
enum class HashType { INVALID = -1, IDENTITY = 0, SHA2_256 = 0X12 };
constexpr std::uint16_t MaximumHashLength = 127;

HashType Validate(HashType);
std::string_view GetName(HashType);
class MultiHash {
 public:
  MultiHash() = default;
  explicit MultiHash(ByteView);
  explicit MultiHash(HashType, ByteView digest);
  
  bool ReadPrefix(ByteView&);

  bool valid() const;
  HashType type() const { return type_; }
  ByteView digest() const { return hash_; }

 private:
  HashType type_ = HashType::INVALID;
  std::vector<Byte> hash_;
};
}  // namespace ipfs

#endif  // IPFS_MULTI_HASH_H_
