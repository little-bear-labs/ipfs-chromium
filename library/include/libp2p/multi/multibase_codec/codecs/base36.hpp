/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_BASE36_HPP
#define LIBP2P_BASE36_HPP

#include "base_error.hpp"

#include <vocab/expected.h>
#include <libp2p/common/types.hpp>

/**
 * Encode/decode to/from base36 format
 */
namespace libp2p::multi::detail {

/**
 * Encode bytes to base36 uppercase string
 * @param bytes to be encoded
 * @return encoded string
 */
std::string encodeBase36Upper(ipfs::ByteView bytes);
/**
 * Encode bytes to base36 lowercase string
 * @param bytes to be encoded
 * @return encoded string
 */
std::string encodeBase36Lower(ipfs::ByteView bytes);

/**
 * Decode base36 (case-insensitively) to bytes
 * @param string to be decoded
 * @return decoded bytes in case of success
 */
ipfs::expected<common::ByteArray, BaseError> decodeBase36(
    std::string_view string);

}  // namespace libp2p::multi::detail

#endif  // LIBP2P_BASE36_HPP
