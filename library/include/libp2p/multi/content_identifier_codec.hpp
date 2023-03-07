/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_CONTENT_IDENTIFIER_CODEC_HPP
#define LIBP2P_CONTENT_IDENTIFIER_CODEC_HPP

#include <libp2p/multi/content_identifier.hpp>
#include <libp2p/multi/multibase_codec/codecs/base58.hpp>

namespace libp2p::multi {

/**
 * Serializes and deserializes CID to byte representation.
 * To serialize it to a multibase encoded string, use MultibaseCodec
 * @see MultibaseCodec
 */
class ContentIdentifierCodec {
 public:
  enum class EncodeError {
    INVALID_CONTENT_TYPE = 1,
    INVALID_HASH_TYPE,
    INVALID_HASH_LENGTH,
    VERSION_UNSUPPORTED,
    INVALID_BASE_ENCODING,
  };

  enum class DecodeError {
    EMPTY_VERSION = 1,
    EMPTY_MULTICODEC,
    MALFORMED_VERSION,
    RESERVED_VERSION,
    CID_TOO_SHORT,
    BAD_MULTIHASH,
    BAD_MULTIBASE
  };

  static ipfs::expected<std::vector<ipfs::Byte>, EncodeError> encode(
      const ContentIdentifier& cid);

  /// Encodes arbitrary byte buffer into CID v.0 wire format
  static std::vector<ipfs::Byte> encodeCIDV0(const void* byte_buffer,
                                             size_t sz);

  /// Encodes arbitrary byte buffer into CID v.1 wire format
  static std::vector<ipfs::Byte> encodeCIDV1(MulticodecType::Code content_type,
                                             const Multihash& mhash);

  static ipfs::expected<ContentIdentifier, DecodeError> decode(
      ipfs::ByteView bytes);

  /**
   * @brief Encode CID to string representation
   * @param cid - input CID for encode
   * Encoding:
   * Base58 for CID v0
   * Base32 for CID v1
   * @return CID string
   */
  static ipfs::expected<std::string, EncodeError> toString(
      const ContentIdentifier& cid);

  /**
   * @brief Encode CID to string representation
   * @param cid - input CID for encode
   * @param base - type of the encoding
   * @return CID string
   */
  static ipfs::expected<std::string, EncodeError> toStringOfBase(
      const ContentIdentifier& cid,
      MultibaseCodec::Encoding base);

  /**
   * @brief Decode string representation of CID to CID
   * @param str - CID string representation
   * @return CID
   */
  static ipfs::expected<ContentIdentifier, DecodeError> fromString(
      const std::string& str);
};

}  // namespace libp2p::multi

std::ostream& operator<<(std::ostream&,
                         libp2p::multi::ContentIdentifierCodec::EncodeError);
std::ostream& operator<<(std::ostream&,
                         libp2p::multi::ContentIdentifierCodec::DecodeError);

#include <vocab/stringify.h>
// OUTCOME_HPP_DECLARE_ERROR(libp2p::multi,
// ContentIdentifierCodec::EncodeError);
// OUTCOME_HPP_DECLARE_ERROR(libp2p::multi,
// ContentIdentifierCodec::DecodeError);

#endif  // LIBP2P_CONTENT_IDENTIFIER_CODEC_HPP
