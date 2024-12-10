#include "ipfs_client/crypto/signing_key_type.h"

#if __has_include(<third_party/ipfs_client/keys.pb.h>)
#include <third_party/ipfs_client/keys.pb.h>
#else
#include "ipfs_client/keys.pb.h"
#endif

using T = ipfs::crypto::SigningKeyType;
namespace n = ipfs::ipns;

// It is critically important that these 2 enumerations remain in-synch.
//   However, some headers that reference SigningKeyType need to be able to
//   compile without access to protobuf.
static_assert(static_cast<int>(T::RSA) == n::RSA);
static_assert(static_cast<int>(T::Ed25519) == n::Ed25519);
static_assert(static_cast<int>(T::Secp256k1) == n::Secp256k1);
static_assert(static_cast<int>(T::ECDSA) == n::ECDSA);
static_assert(static_cast<int>(T::KeyTypeCount) == n::KeyType_ARRAYSIZE);
