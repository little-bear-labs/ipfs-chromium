#ifndef IPFS_HASHER_H_
#define IPFS_HASHER_H_

#include <vocab/byte_view.h>

#include <optional>
#include <vector>

namespace ipfs::crypto {
/*! Base for hash algorithms
 */
class Hasher {
 public:
  virtual ~Hasher() noexcept {}

  virtual std::optional<std::vector<Byte>> hash(ByteView) = 0;
};
}  // namespace ipfs::crypto

#endif  // IPFS_HASHER_H_
