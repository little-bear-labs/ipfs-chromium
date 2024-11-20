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

  /*! @param bytes Bytes to hash
   *  @return The digest or nullopt if there was an error
   */
  virtual std::optional<std::vector<Byte>> hash(ByteView bytes) = 0;
};
}  // namespace ipfs::crypto

#endif  // IPFS_HASHER_H_
