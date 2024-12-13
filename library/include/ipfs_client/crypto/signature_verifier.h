#ifndef IPFS_CHROMIUM_SIGNATURE_VERIFIER_H
#define IPFS_CHROMIUM_SIGNATURE_VERIFIER_H

#include <vocab/byte_view.h>

namespace ipfs::crypto {
/*! Base class for signature-verification algorithms of various type.
 *  @note Gets registered keyed by which algo it handles,
 *    so that much is a prerequisite and can be simply assumed.
 */
class SignatureVerifier {
 public:
  virtual ~SignatureVerifier() noexcept {}
  /*! Non-owned bytes
   */
  using ByteView = ipfs::ByteView;
  virtual bool VerifySignature(ByteView signature,
                               ByteView data,
                               ByteView key) = 0;
};
}  // namespace ipfs::crypto

#endif  // IPFS_CHROMIUM_SIGNATURE_VERIFIER_H
