/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libp2p/multi/content_identifier_codec.hpp"
#include <libp2p/crypto/sha/sha256.hpp>
#include <libp2p/multi/multibase_codec/multibase_codec_impl.hpp>
#include <libp2p/multi/uvarint.hpp>
#include "libp2p/multi/multicodec_type.hpp"

#include <vocab/log_macros.h>

#include <algorithm>

std::ostream& operator<<(std::ostream& str,
                         libp2p::multi::ContentIdentifierCodec::EncodeError e) {
  using E = libp2p::multi::ContentIdentifierCodec::EncodeError;
  switch (e) {
    case E::INVALID_CONTENT_TYPE:
      return str << "Content type does not conform the version";
    case E::INVALID_HASH_LENGTH:
      return str << "Hash length is invalid; Must be 32 bytes for sha256 in "
                    "version 0";
    case E::INVALID_HASH_TYPE:
      return str << "Hash type is invalid; Must be sha256 in version 0";
    case E::INVALID_BASE_ENCODING:
      return str << "Invalid base encoding";
    case E::VERSION_UNSUPPORTED:
      return str << "Content identifier version unsupported";
    default:
      L_DIE("Invalid EncodeError: " << static_cast<unsigned>(e));
  }
}
std::ostream& operator<<(std::ostream& str,
                         libp2p::multi::ContentIdentifierCodec::DecodeError e) {
  using E = libp2p::multi::ContentIdentifierCodec::DecodeError;
  switch (e) {
    case E::EMPTY_MULTICODEC:
      return str << "Multicodec prefix is absent";
    case E::EMPTY_VERSION:
      return str << "Version is absent";
    case E::MALFORMED_VERSION:
      return str << "Version is malformed; Must be a non-negative integer";
    case E::RESERVED_VERSION:
      return str << "Version is greater than the latest version";
    case E::CID_TOO_SHORT:
      return str << "CID too short";
    case E::BAD_MULTIHASH:
      return str << "Bad multihash input";
    case E::BAD_MULTIBASE:
      return str << "Bad multibase input";
    default:
      L_DIE("Invalid decode error " << static_cast<unsigned>(e));
  }
}
namespace libp2p::multi {

ipfs::expected<std::vector<ipfs::Byte>, ContentIdentifierCodec::EncodeError>
ContentIdentifierCodec::encode(const ContentIdentifier& cid) {
  std::vector<ipfs::Byte> bytes;
  if (cid.version == ContentIdentifier::Version::V1) {
    UVarint version(static_cast<uint64_t>(cid.version));
    common::append(bytes, version.toBytes());
    UVarint type(static_cast<uint64_t>(cid.content_type));
    common::append(bytes, type.toBytes());
    auto const& hash = cid.content_address.toBuffer();
    common::append(bytes, hash);

  } else if (cid.version == ContentIdentifier::Version::V0) {
    if (cid.content_type != MulticodecType::Code::DAG_PB) {
      return ipfs::unexpected<EncodeError>{EncodeError::INVALID_CONTENT_TYPE};
    }
    if (cid.content_address.getType() != HashType::sha256) {
      return ipfs::unexpected<EncodeError>{EncodeError::INVALID_HASH_TYPE};
    }
    if (cid.content_address.getHash().size() != 32) {
      return ipfs::unexpected<EncodeError>{EncodeError::INVALID_HASH_LENGTH};
    }
    auto const& hash = cid.content_address.toBuffer();
    common::append(bytes, hash);
  }
  return bytes;
}

std::vector<ipfs::Byte> ContentIdentifierCodec::encodeCIDV0(
    const void* byte_buffer,
    size_t sz) {
  auto digest_res = crypto::sha256(ipfs::ByteView{
      reinterpret_cast<ipfs::Byte const*>(byte_buffer), sz});  // NOLINT
  //  BOOST_ASSERT(digest_res.has_value());
  // TODO DCHECK
  auto& hash = digest_res.value();

  std::vector<ipfs::Byte> bytes;
  bytes.reserve(hash.size());
  bytes.push_back(static_cast<ipfs::Byte>(HashType::sha256));
  bytes.push_back(static_cast<ipfs::Byte>(hash.size()));
  std::transform(hash.begin(), hash.end(), std::back_inserter(bytes),
                 [](auto b) { return ipfs::Byte{b}; });
  return bytes;
}

std::vector<ipfs::Byte> ContentIdentifierCodec::encodeCIDV1(
    MulticodecType::Code content_type,
    const Multihash& mhash) {
  std::vector<ipfs::Byte> bytes;
  // Reserve space for CID version size + content-type size + multihash size
  bytes.reserve(1 + 1 + mhash.toBuffer().size());
  bytes.push_back(ipfs::Byte{1});                          // CID version
  bytes.push_back(static_cast<ipfs::Byte>(content_type));  // Content-Type
  std::copy(mhash.toBuffer().begin(), mhash.toBuffer().end(),
            std::back_inserter(bytes));  // multihash data
  return bytes;
}

auto ContentIdentifierCodec::decode(ipfs::ByteView bytes)
    -> ipfs::expected<ContentIdentifier, DecodeError> {
  if (bytes.size() == 34 and bytes[0] == ipfs::Byte{0x12} and
      bytes[1] == ipfs::Byte{0x20}) {
    auto hash = Multihash::createFromBytes(bytes);
    if (hash.has_value()) {
      return ContentIdentifier(ContentIdentifier::Version::V0,
                               MulticodecType::Code::DAG_PB,
                               std::move(hash.value()));
    } else {
      return ipfs::unexpected<DecodeError>{DecodeError::BAD_MULTIHASH};
    }
  }
  auto version_opt = UVarint::create(bytes);
  if (!version_opt) {
    return ipfs::unexpected<DecodeError>{DecodeError::EMPTY_VERSION};
  }
  auto version = version_opt.value().toUInt64();
  if (version == 1) {
    auto version_length = UVarint::calculateSize(bytes);
    auto multicodec_opt =
        UVarint::create(bytes.subspan(static_cast<ptrdiff_t>(version_length)));
    if (!multicodec_opt) {
      return ipfs::unexpected<DecodeError>{DecodeError::EMPTY_MULTICODEC};
    }
    auto multicodec_length = UVarint::calculateSize(
        bytes.subspan(static_cast<ptrdiff_t>(version_length)));
    auto hash = Multihash::createFromBytes(
        bytes.subspan(version_length + multicodec_length));
    if (hash.has_value()) {
      return ContentIdentifier(
          ContentIdentifier::Version::V1,
          MulticodecType::Code(multicodec_opt.value().toUInt64()),
          std::move(hash.value()));
    } else {
      return ipfs::unexpected<DecodeError>{DecodeError::BAD_MULTIHASH};
    }
  }
  if (version <= 0) {
    return ipfs::unexpected<DecodeError>{DecodeError::MALFORMED_VERSION};
  }
  return ipfs::unexpected<DecodeError>{DecodeError::RESERVED_VERSION};
}

ipfs::expected<std::string, ContentIdentifierCodec::EncodeError>
ContentIdentifierCodec::toString(const ContentIdentifier& cid) {
  std::string result;
  auto encode_result = encode(cid);
  if (!encode_result.has_value()) {
    return ipfs::unexpected<EncodeError>{encode_result.error()};
  }
  auto& cid_bytes = encode_result.value();
  switch (cid.version) {
    case ContentIdentifier::Version::V0:
      result = detail::encodeBase58(cid_bytes);
      break;
    case ContentIdentifier::Version::V1:
      result = MultibaseCodecImpl().encode(
          cid_bytes, MultibaseCodec::Encoding::BASE32_LOWER);
      break;
    default:
      return ipfs::unexpected<EncodeError>{EncodeError::VERSION_UNSUPPORTED};
  }
  return result;
}

auto ContentIdentifierCodec::toStringOfBase(const ContentIdentifier& cid,
                                            MultibaseCodec::Encoding base)
    -> ipfs::expected<std::string, EncodeError> {
  std::string result;
  auto enc_res = encode(cid);
  if (!enc_res.has_value()) {
    return ipfs::unexpected<EncodeError>{enc_res.error()};
  }
  auto& cid_bytes = enc_res.value();
  switch (cid.version) {
    case ContentIdentifier::Version::V0:
      if (base != MultibaseCodec::Encoding::BASE58) {
        return ipfs::unexpected<EncodeError>{
            EncodeError::INVALID_BASE_ENCODING};
      }
      result = detail::encodeBase58(cid_bytes);
      break;
    case ContentIdentifier::Version::V1:
      result = MultibaseCodecImpl().encode(cid_bytes, base);
      break;
    default:
      return ipfs::unexpected<EncodeError>{EncodeError::VERSION_UNSUPPORTED};
  }
  return result;
}

auto ContentIdentifierCodec::fromString(const std::string& str)
    -> ipfs::expected<ContentIdentifier, DecodeError> {
  if (str.size() < 2) {
    return ipfs::unexpected<DecodeError>{DecodeError::CID_TOO_SHORT};
  }

  if (str.size() == 46 && str.substr(0, 2) == "Qm") {
    auto hash = detail::decodeBase58(str);
    if (hash.has_value()) {
      return decode(hash.value());
    } else {
      return ipfs::unexpected<DecodeError>{DecodeError::BAD_MULTIHASH};
    }
  }
  auto bytes = MultibaseCodecImpl().decode(str);
  if (bytes.has_value()) {
    return decode(bytes.value());
  } else {
    return ipfs::unexpected<DecodeError>{DecodeError::BAD_MULTIBASE};
  }
}
}  // namespace libp2p::multi
