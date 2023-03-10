/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_MULTIBASE_IMPL_HPP
#define LIBP2P_MULTIBASE_IMPL_HPP

#include <libp2p/multi/multibase_codec.hpp>

#include <vocab/expected.h>

namespace libp2p::multi {
/**
 * Implementation of the MultibaseCodec with fair codecs
 */
class MultibaseCodecImpl : public MultibaseCodec {
 public:
  ~MultibaseCodecImpl() override = default;

  std::string encode(const ByteBuffer& bytes, Encoding encoding) const override;

  FactoryResult decode(std::string_view string) const override;
};
}  // namespace libp2p::multi

// OUTCOME_HPP_DECLARE_ERROR(libp2p::multi, MultibaseCodecImpl::Error)

#endif  // LIBP2P_MULTIBASE_IMPL_HPP
