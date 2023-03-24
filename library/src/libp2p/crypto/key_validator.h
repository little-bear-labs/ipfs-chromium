/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_CRYPTO_KEY_VALIDATOR_KEY_VALIDATOR_IMPL_HPP
#define LIBP2P_CRYPTO_KEY_VALIDATOR_KEY_VALIDATOR_IMPL_HPP

namespace libp2p::crypto {

class PrivateKey;
class PublicKey;
class KeyPair;

bool validate(PrivateKey const& key);
bool validate(PublicKey const& key);
bool validate(KeyPair const& keys);

}  // namespace libp2p::crypto

#endif  // LIBP2P_CRYPTO_KEY_VALIDATOR_KEY_VALIDATOR_IMPL_HPP
