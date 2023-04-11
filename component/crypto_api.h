#ifndef IPFS_VALIDATE_SIGNATURE_H_
#define IPFS_VALIDATE_SIGNATURE_H_

#include "components/webcrypto/algorithm_implementation.h"

#if __has_include("components/ipfs/keys.pb.h")
#include "components/ipfs/keys.pb.h"
#else
#include "keys.pb.h"
#endif

#include <vocab/byte_view.h>

namespace ipfs::crypto_api {
/*
using Algo = std::pair<blink::WebCryptoAlgorithmId,
                       std::unique_ptr<webcrypto::AlgorithmImplementation>>;
Algo GetAlgo(ipfs::ipns::KeyType);
*/
bool VerifySignature(ipfs::ipns::KeyType,
                     ByteView signature,
                     ByteView data,
                     ByteView key);
}  // namespace ipfs::crypto_api

#endif  // IPFS_VALIDATE_SIGNATURE_H_
