#ifndef IPFS_SIGNING_KEY_TYPE_H_
#define IPFS_SIGNING_KEY_TYPE_H_

namespace ipfs::crypto {
enum class SigningKeyType : int {
  RSA,
  Ed25519,
  Secp256k1,
  ECDSA,
  KeyTypeCount
};
}

#endif  // IPFS_SIGNING_KEY_TYPE_H_
