/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_SRC_CRYPTO_HASHER_HPP
#define LIBP2P_SRC_CRYPTO_HASHER_HPP

#include <libp2p/crypto/common.hpp>
#include <libp2p/crypto/error.hpp>

#include "vocab/byte_view.h"
#include "vocab/expected.h"

namespace libp2p::crypto {

using libp2p::crypto::common::HashType;

class Hasher {
 public:
  virtual ~Hasher() = default;

  using Result = ipfs::expected<bool, Error>;

  /// appends a new chunk of data
  virtual Result write(ipfs::ByteView data) = 0;

  /**
   * Calculates the current digest.
   * Does not affect the internal state.
   * New data still could be fed via write method.
   */
  virtual Result digestOut(ipfs::span<ipfs::Byte> out) const = 0;

  /// resets the internal state
  virtual Result reset() = 0;

  /// hash size in bytes
  virtual size_t digestSize() const = 0;

  /// block size in bytes for the most optimal hash update via write method
  virtual size_t blockSize() const = 0;

  /// runtime identifiable hasher type
  virtual HashType hashType() const = 0;

  ipfs::expected<std::vector<ipfs::Byte>, Error> digest() const;
};
}  // namespace libp2p::crypto

#endif  // LIBP2P_SRC_CRYPTO_HASHER_HPP
