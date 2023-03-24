/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "key_marshaller.h"
#include "key_validator.h"

#include <libp2p/crypto/common.hpp>

#if __has_include("components/ipfs/ipns_record.pb.h")
#include "components/ipfs/keys.pb.h"
#else
#include "keys.pb.h"
#endif

using libp2p::crypto::CryptoProviderError;
using libp2p::crypto::Key;

namespace {
/**
 * @brief converts Key::Type to ipfs::ipns::KeyType
 * @param type common key type value
 * @return protobuf key type value
 */
ipfs::expected<ipfs::ipns::KeyType, CryptoProviderError> marshalKeyType(
    Key::Type type) {
  switch (type) {
    case Key::Type::RSA:
      return ipfs::ipns::KeyType::RSA;
    case Key::Type::Ed25519:
      return ipfs::ipns::KeyType::Ed25519;
    case Key::Type::Secp256k1:
      return ipfs::ipns::KeyType::Secp256k1;
    case Key::Type::ECDSA:
      return ipfs::ipns::KeyType::ECDSA;
    case Key::Type::UNSPECIFIED:
      return CryptoProviderError::INVALID_KEY_TYPE;
  }

  return CryptoProviderError::UNKNOWN_KEY_TYPE;
}

/**
 * @brief converts ipfs::ipns::KeyType to Key::Type
 * @param type protobuf key type value
 * @return common key type value
 */
ipfs::expected<Key::Type, CryptoProviderError> unmarshalKeyType(
    ipfs::ipns::KeyType type) {
  switch (type) {
    case ipfs::ipns::KeyType::RSA:
      return Key::Type::RSA;
    case ipfs::ipns::KeyType::Ed25519:
      return Key::Type::Ed25519;
    case ipfs::ipns::KeyType::Secp256k1:
      return Key::Type::Secp256k1;
    case ipfs::ipns::KeyType::ECDSA:
      return Key::Type::ECDSA;
    default:
      return CryptoProviderError::UNKNOWN_KEY_TYPE;
  }
}
}  // namespace

using libp2p::crypto::KeyMarshaller;

KeyMarshaller::KeyMarshaller() {}

#undef OUTCOME_TRY
#define OUTCOME_TRY(var, ...)                             \
  auto var##_result = __VA_ARGS__;                        \
  if (!var##_result.has_value()) {                        \
    return ipfs::unexpected<Error>{var##_result.error()}; \
  }                                                       \
  auto& var = var##_result.value();

auto KeyMarshaller::marshal(const PublicKey& key) const
    -> ipfs::expected<ProtobufKey, Error> {
  ipfs::ipns::PublicKey protobuf_key;
  OUTCOME_TRY(type, marshalKeyType(key.type));
  protobuf_key.set_type(type);
  protobuf_key.set_data(key.data.data(), key.data.size());

  auto string = protobuf_key.SerializeAsString();
  return ProtobufKey{{string.begin(), string.end()}};
}

auto KeyMarshaller::marshal(const PrivateKey& key) const
    -> ipfs::expected<ProtobufKey, Error> {
  ipfs::ipns::PrivateKey protobuf_key;
  OUTCOME_TRY(type, marshalKeyType(key.type));
  protobuf_key.set_type(type);
  protobuf_key.set_data(key.data.data(), key.data.size());

  auto string = protobuf_key.SerializeAsString();
  return ProtobufKey{{string.begin(), string.end()}};
}

auto KeyMarshaller::unmarshalPublicKey(const ProtobufKey& proto_key) const
    -> ipfs::expected<PublicKey, Error> {
  ipfs::ipns::PublicKey protobuf_key;
  if (!protobuf_key.ParseFromArray(proto_key.key.data(),
                                   static_cast<int>(proto_key.key.size()))) {
    return CryptoProviderError::FAILED_UNMARSHAL_DATA;
  }

  OUTCOME_TRY(type, unmarshalKeyType(protobuf_key.type()));
  auto key = PublicKey{
      {type, {protobuf_key.data().begin(), protobuf_key.data().end()}}};

  if (validate(key)) {
    return key;
  } else {
    return KeyValidatorError::INVALID_PUBLIC_KEY;
  }
}

auto KeyMarshaller::unmarshalPrivateKey(const ProtobufKey& proto_key) const
    -> ipfs::expected<PrivateKey, Error> {
  ipfs::ipns::PublicKey protobuf_key;
  if (!protobuf_key.ParseFromArray(proto_key.key.data(),
                                   static_cast<int>(proto_key.key.size()))) {
    return CryptoProviderError::FAILED_UNMARSHAL_DATA;
  }

  OUTCOME_TRY(type, unmarshalKeyType(protobuf_key.type()));
  auto key = PrivateKey{
      {type, {protobuf_key.data().begin(), protobuf_key.data().end()}}};

  if (validate(key)) {
    return key;
  } else {
    return KeyValidatorError::INVALID_PRIVATE_KEY;
  }
}
