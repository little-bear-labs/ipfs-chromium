#ifndef IPFS_VALIDATE_SIGNATURE_H_
#define IPFS_VALIDATE_SIGNATURE_H_

#include <ipfs_client/context_api.h>
#include <vocab/byte_view.h>
#include "components/webcrypto/algorithm_implementation.h"

namespace ipfs::crypto_api {

bool VerifySignature(ipfs::ContextApi::SigningKeyType,
                     ByteView signature,
                     ByteView data,
                     ByteView key);

}  // namespace ipfs::crypto_api

#endif  // IPFS_VALIDATE_SIGNATURE_H_
