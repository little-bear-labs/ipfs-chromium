#ifndef IPFS_OPENSSL_SHA2_256_H_
#define IPFS_OPENSSL_SHA2_256_H_

#if __has_include(<openssl/sha.h>)
#define HAS_OPENSSL_SHA 1
#endif

#include <ipfs_client/crypto/hasher.h>

namespace ipfs::crypto {
/*! Implement SHA-256 hashing
 */
class OpensslSha2_256 final : public Hasher {
 public:
  ~OpensslSha2_256() noexcept override;
  std::optional<std::vector<Byte>> hash(ByteView) override;
};
}  // namespace ipfs::crypto

#endif  // IPFS_OPENSSL_SHA2_256_H_
