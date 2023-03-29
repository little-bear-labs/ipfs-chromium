/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_CRYPTO_MARSHALER_KEY_MARSHALER_IMPL_HPP
#define LIBP2P_CRYPTO_MARSHALER_KEY_MARSHALER_IMPL_HPP

#include <memory>

#include "protobuf_key.hpp"

#include <libp2p/crypto/error.hpp>
#include "libp2p/crypto/key.h"

#include <vocab/expected.h>

namespace libp2p::crypto {
class KeyValidator;

// class KeyMarshaller : public KeyMarshaller {
class KeyMarshaller {
 public:
  explicit KeyMarshaller();

  ~KeyMarshaller() = default;

  ipfs::expected<ProtobufKey, Error> marshal(const PublicKey& key) const;

  ipfs::expected<ProtobufKey, Error> marshal(const PrivateKey& key) const;

  ipfs::expected<PublicKey, Error> unmarshalPublicKey(
      const ProtobufKey& key) const;

  ipfs::expected<PrivateKey, Error> unmarshalPrivateKey(
      const ProtobufKey& key) const;
};
}  // namespace libp2p::crypto

#endif  // LIBP2P_CRYPTO_MARSHALER_KEY_MARSHALER_IMPL_HPP
