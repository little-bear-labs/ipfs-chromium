#include "ipfs_client/crypto/openssl_signature_verifier.h"
#include <openssl/ossl_typ.h>
#include <openssl/evp.h>

#include <memory>

#include "log_macros.h"
#include "vocab/byte_view.h"

#if HAS_OPENSSL_EVP

using Self = ipfs::crypto::OpensslSignatureVerifier;

Self::OpensslSignatureVerifier(int key_type) : openssl_key_type_{key_type} {}
Self::~OpensslSignatureVerifier() noexcept = default;
auto Self::VerifySignature(ipfs::ByteView signature,
                           ipfs::ByteView data,
                           ipfs::ByteView key_bytes) -> bool {
  const auto* key_p = reinterpret_cast<unsigned char const*>(key_bytes.data());
  const auto* data_p = reinterpret_cast<unsigned char const*>(data.data());
  const auto* sig_p = reinterpret_cast<unsigned char const*>(signature.data());
  auto kt = openssl_key_type_;
  std::unique_ptr<EVP_MD_CTX, void (*)(EVP_MD_CTX*)> const ctx{EVP_MD_CTX_new(),
                                                         &EVP_MD_CTX_free};
  std::unique_ptr<EVP_PKEY, void (*)(EVP_PKEY*)> const key{
      EVP_PKEY_new_raw_public_key(kt, nullptr, key_p, key_bytes.size()),
      &EVP_PKEY_free};
  if (EVP_DigestVerifyInit(ctx.get(), nullptr, nullptr, nullptr, key.get()) == 0) {
    LOG(ERROR) << "EVP_DigestVerifyInit failed";
    return false;
  }
  auto result =
      EVP_DigestVerify(ctx.get(), sig_p, signature.size(), data_p, data.size());
  return result == 1;
}

#endif
