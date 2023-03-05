/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_SHA256_HPP
#define LIBP2P_SHA256_HPP

#include <openssl/sha.h>
#include <libp2p/crypto/hasher.hpp>
#include "libp2p/common/types.hpp"

namespace libp2p::crypto {

class Sha256 : public Hasher {
 public:
  Sha256();

  ~Sha256() override;

  Result write(ipfs::ByteView data) override;

  Result digestOut(ipfs::span<ipfs::Byte> out) const override;

  Result reset() override;

  size_t digestSize() const override;

  size_t blockSize() const override;

  HashType hashType() const override;

 private:
  void sinkCtx();

  SHA256_CTX ctx_;
  bool initialized_;
};

/**
 * Take a SHA-256 hash from bytes
 * @param input to be hashed
 * @return hashed bytes
 */
ipfs::expected<libp2p::common::Hash256, Error> sha256(ipfs::ByteView input);
}  // namespace libp2p::crypto

#endif  // LIBP2P_SHA256_HPP
