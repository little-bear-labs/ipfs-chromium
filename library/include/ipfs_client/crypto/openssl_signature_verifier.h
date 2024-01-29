#ifndef IPFS_CHROMIUM_OPENSSL_SIGNATURE_VERIFIER_H
#define IPFS_CHROMIUM_OPENSSL_SIGNATURE_VERIFIER_H

#include "signature_verifier.h"

#ifdef _MSC_VER

// TODO - issues with the version of OpenSSL I'm getting from Conan
#pragma message("This signature verifier not ported to MSVC")

#elif __has_include(<openssl/evp.h>)
#include <openssl/evp.h>
#define HAS_OPENSSL_EVP 1
#endif

namespace ipfs::crypto {

#if HAS_OPENSSL_EVP

class OpensslSignatureVerifier : public SignatureVerifier {
  int const openssl_key_type_;

 public:
  OpensslSignatureVerifier(int);
  ~OpensslSignatureVerifier() noexcept override;

  bool VerifySignature(ByteView signature,
                       ByteView data,
                       ByteView key) override;
};

#endif

}  // namespace ipfs::crypto

#endif  // IPFS_CHROMIUM_OPENSSL_SIGNATURE_VERIFIER_H
