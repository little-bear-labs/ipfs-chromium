/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_BASE58_HPP
#define LIBP2P_BASE58_HPP

#include "base_error.hpp"

#include <vocab/expected.h>
#include <libp2p/common/types.hpp>

#include <optional>
#include <string>

/**
 * Encode/decode to/from base58 format
 * Implementation is taken from
 * https://github.com/bitcoin/bitcoin/blob/master/src/base58.h
 */
namespace libp2p::multi::detail {

/**
 * Encode bytes to base58 string
 * @param bytes to be encoded
 * @return encoded string
 */
std::string encodeBase58(const common::ByteArray& bytes);

/**
 * Decode base58 string to bytes
 * @param string to be decoded
 * @return decoded bytes in case of success
 */
ipfs::expected<common::ByteArray, BaseError> decodeBase58(
    std::string_view string);
}  // namespace libp2p::multi::detail

#endif  // LIBP2P_BASE58_HPP
