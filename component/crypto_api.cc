#include "crypto_api.h"

#include "base/logging.h"
#include "components/webcrypto/algorithm_implementations.h"
#include "components/webcrypto/status.h"
#include "third_party/blink/public/platform/web_crypto_key.h"
#include "third_party/boringssl/src/include/openssl/evp.h"

namespace {
int ToEvpKeyType(ipfs::ContextApi::SigningKeyType t) {
  using T = ipfs::ContextApi::SigningKeyType;
  switch (t) {
    case T::ECDSA:
      LOG(ERROR) << "TODO Check on ECDSA key type translation.";
      return EVP_PKEY_EC;
    case T::Ed25519:
      return EVP_PKEY_ED25519;
    case T::RSA:
      return EVP_PKEY_RSA;
    case T::Secp256k1:
      LOG(ERROR) << "TODO Check on Secp256k1 key type translation.";
      return EVP_PKEY_DSA;
    default:
      LOG(ERROR) << "Invalid key type: " << static_cast<long>(t);
      return EVP_PKEY_NONE;
  }
}
}  // namespace

namespace cpto = ipfs::crypto_api;

bool cpto::VerifySignature(ipfs::ContextApi::SigningKeyType key_type,
                           ipfs::ByteView signature,
                           ipfs::ByteView data,
                           ipfs::ByteView key_bytes) {
  auto* key_p = reinterpret_cast<unsigned char const*>(key_bytes.data());
  auto* data_p = reinterpret_cast<unsigned char const*>(data.data());
  auto* sig_p = reinterpret_cast<unsigned char const*>(signature.data());
  auto kt = ToEvpKeyType(key_type);
  std::clog << "data:";
  for (auto b : data) {
    std::clog << ' ' << std::hex << static_cast<unsigned>(b);
  }
  std::clog << ' ' << data.size() << " bytes.\n";
  bssl::UniquePtr<EVP_PKEY> pkey(EVP_PKEY_new_raw_public_key(
      kt, /*engine*/ nullptr, key_p, key_bytes.size()));
  bssl::ScopedEVP_MD_CTX ctx;
  if (!EVP_DigestVerifyInit(ctx.get(), /*pctx=*/nullptr, /*type=*/nullptr,
                            /*e=*/nullptr, pkey.get())) {
    LOG(ERROR) << "EVP_DigestVerifyInit failed";
    return false;
  }
  auto result =
      EVP_DigestVerify(ctx.get(), sig_p, signature.size(), data_p, data.size());
  //                                 to_verify.data(), to_verify.size());
  VLOG(1) << "EVP_DigestVerify returned " << result;
  return result == 1;
}
