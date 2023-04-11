#include "crypto_api.h"

#include "base/logging.h"
#include "components/webcrypto/algorithm_implementations.h"
#include "components/webcrypto/status.h"
#include "third_party/blink/public/platform/web_crypto_key.h"
#include "third_party/boringssl/src/include/openssl/evp.h"

namespace {
int ToEvpKeyType(ipfs::ipns::KeyType t) {
  using ipfs::ipns::KeyType;
  switch (t) {
    case KeyType::ECDSA:
      LOG(ERROR) << "Check on ECDSA key type translation.";
      return EVP_PKEY_EC;
    case KeyType::Ed25519:
      return EVP_PKEY_ED25519;
    case KeyType::RSA:
      return EVP_PKEY_RSA;
    case KeyType::Secp256k1:
      LOG(ERROR) << "Check on Secp256k1 key type translation.";
      return EVP_PKEY_DSA;
    default:
      LOG(ERROR) << "Invalid key type: " << static_cast<long>(t);
      return EVP_PKEY_NONE;
  }
}
}  // namespace

namespace crypt = ipfs::crypto_api;
/*
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
 */
bool crypt::VerifySignature(ipfs::ipns::KeyType key_type,
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
  //  auto* prefix = reinterpret_cast<unsigned char const*>(
  //      "\x69\x70\x6e\x73\x2d\x73\x69\x67\x6e\x61\x74\x75\x72\x65\x3a");
  //  std::basic_string<unsigned char> to_verify = prefix;
  //  to_verify.append(data_p, data.size());
  auto result =
      EVP_DigestVerify(ctx.get(), sig_p, signature.size(), data_p, data.size());
  //                                 to_verify.data(), to_verify.size());
  LOG(INFO) << "EVP_DigestVerify returned " << result;
  return result == 1;
  /*
  auto* pre_p = reinterpret_cast<unsigned char const*>(prefix.data());
  if (!EVP_DigestVerifyUpdate(ctx.get(), pre_p, prefix.size())) {
    LOG(ERROR) << "EVP_DigestVerifyUpdate failed on prefix.";
    return false;
  }
  if (!EVP_DigestVerifyUpdate(ctx.get(), data_p, data.size())) {
    LOG(ERROR) << "EVP_DigestVerifyUpdate failed on actual data.";
    return false;
  }
  auto result = EVP_DigestVerifyFinal(ctx.get(), sig_p, signature.size());
  LOG(INFO) << "EVP_DigestVerifyFinal returned " << result;
  return result == 1;
   */
}
