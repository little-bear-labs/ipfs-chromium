#ifndef IPFS_CHROMIUM_MOCK_SIG_VTOR_H
#define IPFS_CHROMIUM_MOCK_SIG_VTOR_H

#include <ipfs_client/crypto/signature_verifier.h>

namespace {
struct MockSigVtor : public ipfs::crypto::SignatureVerifier {
  bool VerifySignature(ByteView, ByteView, ByteView) { return true; }
};
}  // namespace

#endif  // IPFS_CHROMIUM_MOCK_SIG_VTOR_H
