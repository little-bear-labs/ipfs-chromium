/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_BASE_ERROR_HPP
#define LIBP2P_BASE_ERROR_HPP

namespace libp2p::multi::detail {

enum class BaseError {
  INVALID_BASE58_INPUT = 1,
  INVALID_BASE64_INPUT,
  INVALID_BASE32_INPUT,
  INVALID_BASE36_INPUT,
  NON_UPPERCASE_INPUT,
  NON_LOWERCASE_INPUT
};

}

#endif  // LIBP2P_BASE_ERROR_HPP
