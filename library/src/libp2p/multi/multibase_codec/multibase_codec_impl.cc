/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <libp2p/multi/multibase_codec/multibase_codec_impl.hpp>

#include <vocab/log_macros.h>

#include <unordered_map>

// TODO bring over these other bases
//  #include <libp2p/multi/multibase_codec/codecs/base16.hpp>
#include <libp2p/multi/multibase_codec/codecs/base32.hpp>
#include <libp2p/multi/multibase_codec/codecs/base58.hpp>
// #include <libp2p/multi/multibase_codec/codecs/base64.hpp>

namespace {
using namespace libp2p::multi;          // NOLINT
using namespace libp2p::multi::detail;  // NOLINT

/**
 * Get the encoding by a character
 * @param ch of encoding
 * @return related encoding, if character stands for one of them, none
 * otherwise
 */
std::optional<MultibaseCodec::Encoding> encodingByChar(char ch) {
  switch (ch) {
    case 'f':
      return MultibaseCodec::Encoding::BASE16_LOWER;
    case 'F':
      return MultibaseCodec::Encoding::BASE16_UPPER;
    case 'b':
      return MultibaseCodec::Encoding::BASE32_LOWER;
    case 'B':
      return MultibaseCodec::Encoding::BASE32_UPPER;
    case 'z':
      return MultibaseCodec::Encoding::BASE58;
    case 'm':
      return MultibaseCodec::Encoding::BASE64;
    default:
      return std::nullopt;
  }
}

struct CodecFunctions {
  using EncodeFuncType = decltype(encodeBase58);
  using DecodeFuncType = decltype(decodeBase58);

  EncodeFuncType* encode;
  DecodeFuncType* decode;
};

namespace {
std::string todo_encode(libp2p::common::ByteArray const&) {
  LOG(FATAL) << "TODO implement encode for this multibase encoding";
  return "TODO";
}
ipfs::expected<libp2p::common::ByteArray, BaseError> todo_decode(
    std::string_view string) {
  LOG(FATAL) << "TODO implement encode for this multibase encoding "
             << std::string{string};
  return ipfs::unexpected<BaseError>{BaseError::INVALID_BASE32_INPUT};
}
}  // namespace

/// all available codec functions
const std::unordered_map<MultibaseCodec::Encoding, CodecFunctions> codecs{
    {MultibaseCodec::Encoding::BASE16_UPPER,
     CodecFunctions{&todo_encode, &todo_decode}},
    {MultibaseCodec::Encoding::BASE16_LOWER,
     CodecFunctions{&todo_encode, &todo_decode}},
    {MultibaseCodec::Encoding::BASE32_UPPER,
     CodecFunctions{&encodeBase32Upper, &decodeBase32Upper}},
    {MultibaseCodec::Encoding::BASE32_LOWER,
     CodecFunctions{&encodeBase32Lower, &decodeBase32Lower}},
    {MultibaseCodec::Encoding::BASE58,
     CodecFunctions{&encodeBase58, &decodeBase58}},
    {MultibaseCodec::Encoding::BASE64,
     CodecFunctions{&todo_encode, &todo_decode}}};
}  // namespace
/*
OUTCOME_CPP_DEFINE_CATEGORY(libp2p::multi, MultibaseCodecImpl::Error, e) {
  using E = libp2p::multi::MultibaseCodecImpl::Error;
  switch (e) {
    case E::INPUT_TOO_SHORT:
      return "Input must be at least two bytes long";
    case E::UNSUPPORTED_BASE:
      return "The base is either not supported or does not exist";
    default:
      return "Unknown error";
  }
}
*/
namespace libp2p::multi {
using common::ByteArray;

std::string MultibaseCodecImpl::encode(const ByteArray& bytes,
                                       Encoding encoding) const {
  if (bytes.empty()) {
    return "";
  }

  return static_cast<char>(encoding) + codecs.at(encoding).encode(bytes);
}

auto MultibaseCodecImpl::decode(std::string_view string) const
    -> FactoryResult {
  if (string.length() < 2) {
    return ipfs::unexpected<Error>{Error::INPUT_TOO_SHORT};
  }

  auto encoding_base = encodingByChar(string.front());
  if (!encoding_base) {
    return ipfs::unexpected<Error>{Error::UNSUPPORTED_BASE};
  }

  auto result = codecs.at(*encoding_base).decode(string.substr(1));
  if (result.has_value()) {
    return result.value();
  } else {
    return ipfs::unexpected<Error>{Error::BASE_CODEC_ERROR};
  }
}
}  // namespace libp2p::multi
