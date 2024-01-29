#ifndef IPFS_CHROMIUM_SIGNATURE_VERIFIER_H
#define IPFS_CHROMIUM_SIGNATURE_VERIFIER_H

#include <vocab/byte_view.h>

namespace ipfs::crypto {
class SignatureVerifier {
 public:
  virtual ~SignatureVerifier() noexcept {}
  using ByteView = ipfs::ByteView;
  virtual bool VerifySignature(ByteView signature,
                               ByteView data,
                               ByteView key) = 0;
};
}  // namespace ipfs::crypto

#endif  // IPFS_CHROMIUM_SIGNATURE_VERIFIER_H
