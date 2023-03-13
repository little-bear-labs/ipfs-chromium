/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstring>

#include <libp2p/crypto/sha/sha256.hpp>

#include <openssl/sha.h>
#include <libp2p/crypto/error.hpp>

#if __GNUG__
// OpenSSL wants us to use 3.0
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace libp2p::crypto {
Sha256::Sha256() {  // NOLINT
  initialized_ = 1 == SHA256_Init(&ctx_);
}

Sha256::~Sha256() {
  sinkCtx();
}

auto Sha256::write(ipfs::ByteView data) -> Result {
  if (not initialized_) {
    return ipfs::unexpected<Error>{
        HmacProviderError::FAILED_INITIALIZE_CONTEXT};
  }
  if (1 != SHA256_Update(&ctx_, data.data(), data.size())) {
    return ipfs::unexpected<Error>{HmacProviderError::FAILED_UPDATE_DIGEST};
  }
  return true;
}

auto Sha256::digestOut(ipfs::span<ipfs::Byte> out) const -> Result {
  if (not initialized_) {
    return ipfs::unexpected<Error>{
        HmacProviderError::FAILED_INITIALIZE_CONTEXT};
  }
  if (out.size() != static_cast<std::size_t>(digestSize())) {
    return ipfs::unexpected<Error>{HmacProviderError::WRONG_DIGEST_SIZE};
  }
  SHA256_CTX ctx = ctx_;
  if (1 != SHA256_Final(reinterpret_cast<unsigned char*>(out.data()), &ctx)) {
    return ipfs::unexpected<Error>{HmacProviderError::FAILED_FINALIZE_DIGEST};
  }
  return true;
}

auto Sha256::reset() -> Result {
  sinkCtx();
  if (1 != SHA256_Init(&ctx_)) {
    return ipfs::unexpected<Error>{
        HmacProviderError::FAILED_INITIALIZE_CONTEXT};
  }
  initialized_ = true;
  return true;
}

size_t Sha256::digestSize() const {
  return SHA256_DIGEST_LENGTH;
}

size_t Sha256::blockSize() const {
  return SHA256_CBLOCK;
}

void Sha256::sinkCtx() {
  if (initialized_) {
    libp2p::common::Hash256 digest;
    SHA256_Final(digest.data(), &ctx_);
    memset(digest.data(), 0, digest.size());
    initialized_ = false;
  }
}

HashType Sha256::hashType() const {
  return HashType::SHA256;
}

ipfs::expected<libp2p::common::Hash256, Error> sha256(ipfs::ByteView input) {
  Sha256 sha;
  auto write_res = sha.write(input);
  if (!write_res.has_value()) {
    return ipfs::unexpected<Error>{write_res.error()};
  }
  libp2p::common::Hash256 result;
  auto dig_res = sha.digestOut(
      {reinterpret_cast<ipfs::Byte*>(result.data()), result.size()});
  if (dig_res.has_value()) {
    return result;
  } else {
    return ipfs::unexpected<Error>{dig_res.error()};
  }
}
}  // namespace libp2p::crypto
