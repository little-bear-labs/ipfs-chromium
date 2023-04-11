#include "crypto_api.h"

#include "base/logging.h"
#include "components/webcrypto/algorithm_implementations.h"
#include "components/webcrypto/status.h"
#include "third_party/blink/public/platform/web_crypto_key.h"

namespace crypt = ipfs::crypto_api;

auto crypt::GetAlgo(ipfs::ipns::KeyType t) -> Algo {
  switch (t) {
    case ipfs::ipns::KeyType::Ed25519:
      return {blink::kWebCryptoAlgorithmIdEcdsa,
              webcrypto::CreateEcdhImplementation()};
    default:
      LOG(ERROR) << "Unimplemented key type: " << static_cast<int>(t);
      return {};
  }
}
bool crypt::VerifySignature(ipfs::ipns::KeyType kt,
                            ipfs::ByteView signature,
                            ipfs::ByteView data,
                            ipfs::ByteView key_bytes) {
  auto [algo_id, algo_impl] = GetAlgo(kt);
  if (!algo_impl) {
    LOG(ERROR) << "Unhandled key type, simply assuming the signature is valid.";
    return true;
  }
  auto algo = blink::WebCryptoAlgorithm::AdoptParamsAndCreate(algo_id, nullptr);
  blink::WebCryptoKey key;
  auto status =
      algo_impl->ImportKey(blink::kWebCryptoKeyFormatRaw, as_octets(key_bytes),
                           algo, false, blink::kWebCryptoKeyUsageVerify, &key);
  if (status.IsError()) {
    LOG(ERROR) << "Failed to import key because: " << status.error_details();
    return false;
  }
  auto result = false;
  status = algo_impl->Verify(algo, key, as_octets(signature), as_octets(data),
                             &result);
  if (status.IsError()) {
    LOG(ERROR) << "Failed to validate signature because: "
               << status.error_details();
    return false;
  }
  return result;
}
