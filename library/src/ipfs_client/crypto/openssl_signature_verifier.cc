#include "ipfs_client/crypto/openssl_signature_verifier.h"

#include <memory>

#include "log_macros.h"

#if HAS_OPENSSL_EVP

using Self = ipfs::crypto::OpensslSignatureVerifier;

Self::OpensslSignatureVerifier(int key_type) : openssl_key_type_{key_type} {}
Self::~OpensslSignatureVerifier() {}
bool Self::VerifySignature(ipfs::ByteView signature,
                           ipfs::ByteView data,
                           ipfs::ByteView key_bytes) {
  auto* key_p = reinterpret_cast<unsigned char const*>(key_bytes.data());
  auto* data_p = reinterpret_cast<unsigned char const*>(data.data());
  auto* sig_p = reinterpret_cast<unsigned char const*>(signature.data());
  auto kt = openssl_key_type_;
  std::unique_ptr<EVP_MD_CTX, void (*)(EVP_MD_CTX*)> ctx{EVP_MD_CTX_new(),
                                                         &EVP_MD_CTX_free};
  std::unique_ptr<EVP_PKEY, void (*)(EVP_PKEY*)> key{
      EVP_PKEY_new_raw_public_key(kt, nullptr, key_p, key_bytes.size()),
      &EVP_PKEY_free};
  if (!EVP_DigestVerifyInit(ctx.get(), nullptr, nullptr, nullptr, key.get())) {
    LOG(ERROR) << "EVP_DigestVerifyInit failed";
    return false;
  }
  auto result =
      EVP_DigestVerify(ctx.get(), sig_p, signature.size(), data_p, data.size());
  return result == 1;
}

#endif
