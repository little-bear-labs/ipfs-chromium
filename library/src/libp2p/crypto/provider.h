#ifndef IPFS_PROVIDER_H_
#define IPFS_PROVIDER_H_

#include "libp2p/crypto/key.h"

#include <libp2p/crypto/error.hpp>

#include <vocab/byte_view.h>
#include <vocab/expected.h>

namespace libp2p::crypto {
class Provider {
 public:
  template <class T>
  using Result = ipfs::expected<T, Error>;

  virtual Result<PublicKey> derive(const PrivateKey& private_key) const = 0;

  virtual Result<Signature> sign(ipfs::ByteView message,
                                 const PrivateKey& private_key) const = 0;

  virtual Result<bool> verify(ipfs::ByteView message,
                              const Signature& signature,
                              const PublicKey& key) const = 0;
};
}  // namespace libp2p::crypto

#endif  // IPFS_PROVIDER_H_
