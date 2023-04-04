/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libp2p/multi/multihash.hpp"

#include "libp2p/basic/varint_prefix_reader.hpp"
#include "libp2p/common/hexutil.hpp"
#include "libp2p/common/types.hpp"

#include <google/protobuf/stubs/logging.h>

using libp2p::common::ByteArray;
using libp2p::common::hex_upper;
using libp2p::common::unhex;

std::string libp2p::multi::to_string(Multihash::Error e) {
  using E = libp2p::multi::Multihash::Error;
  switch (e) {
    case E::ZERO_INPUT_LENGTH:
      return "The length encoded in the header is zero";
    case E::INCONSISTENT_LENGTH:
      return "The length encoded in the input data header doesn't match the "
             "actual length of the input data";
    case E::INPUT_TOO_LONG:
      return "The length of the input exceeds the maximum length of " +
             std::to_string(libp2p::multi::Multihash::kMaxHashLength);
    case E::INPUT_TOO_SHORT:
      return "The length of the input is less than the required minimum of two "
             "bytes for the multihash header";
    default:
      return "Unknown error";
  }
}

namespace libp2p::multi {

Multihash::Multihash(HashType type, ipfs::ByteView hash)
    : data_(std::make_shared<const Data>(type, hash)) {}

Multihash::Multihash(const Multihash& other) : data_{other.data_} {}

Multihash::Multihash(Multihash&& other) noexcept : data_{other.data_} {}

Multihash::~Multihash() noexcept {}

namespace {
template <typename Buffer>
inline void appendVarint(Buffer& buffer, ipfs::Byte t) {
  do {
    auto byte = t & ipfs::Byte{0x7F};
    t >>= 7;
    if (t != ipfs::Byte{0}) {
      byte |= ipfs::Byte{0x80};
    }
    buffer.push_back(byte);
  } while (t != ipfs::Byte{0});
}
}  // namespace

Multihash::Data::Data(HashType t, ipfs::ByteView h) : type(t) {
  bytes.reserve(h.size() + 4);
  appendVarint(bytes, static_cast<ipfs::Byte>(type));
  //  BOOST_ASSERT(h.size() <= std::numeric_limits<uint8_t>::max());
  bytes.push_back(static_cast<ipfs::Byte>(h.size()));
  hash_offset = bytes.size();
  bytes.insert(bytes.end(), h.begin(), h.end());
  std_hash = std::hash<std::string_view>{}(std::string_view{
      reinterpret_cast<char const*>(bytes.data()), bytes.size()});
}

Multihash::Data::~Data() noexcept {}

const Multihash::Data& Multihash::data() const {
  GOOGLE_DCHECK(data_);
  return *data_;
}

size_t Multihash::stdHash() const {
  return data().std_hash;
}

using Result = ipfs::expected<Multihash, Multihash::Error>;

Result Multihash::create(HashType type, ipfs::ByteView hash) {
  if (hash.size() > kMaxHashLength) {
    return ipfs::unexpected<Error>(Error::INPUT_TOO_LONG);
  }

  return Multihash{type, hash};
}

Result Multihash::createFromHex(std::string_view hex) {
  auto result = unhex(hex);
  if (result.has_value()) {
    auto view = ipfs::ByteView{result.value().data(), result.value().size()};
    return Multihash::createFromBytes(view);
  } else {
    return ipfs::unexpected<Error>(Error::INVALID_HEXADECIMAL_INPUT);
  }
}

Result Multihash::createFromBytes(ipfs::ByteView b) {
  if (b.size() < kHeaderSize) {
    return ipfs::unexpected<Error>(Error::INPUT_TOO_SHORT);
  }

  basic::VarintPrefixReader vr;
  if (vr.consume(b) != basic::VarintPrefixReader::kReady) {
    return ipfs::unexpected<Error>(Error::INPUT_TOO_SHORT);
  }

  const auto type = static_cast<HashType>(vr.value());
  if (b.empty()) {
    return ipfs::unexpected<Error>(Error::INPUT_TOO_SHORT);
  }

  auto length = to_integer(b[0]);
  ipfs::ByteView hash = b.subspan(1);

  if (length == 0) {
    return ipfs::unexpected<Error>(Error::ZERO_INPUT_LENGTH);
  }

  if (hash.size() != length) {
    return ipfs::unexpected<Error>(Error::INCONSISTENT_LENGTH);
  }

  return Multihash::create(type, hash);
}

const HashType& Multihash::getType() const {
  return data().type;
}

ipfs::ByteView Multihash::getHash() const {
  const auto& d = data();
  return ipfs::ByteView(d.bytes.data(), d.bytes.size()).subspan(d.hash_offset);
}

std::string Multihash::toHex() const {
  return hex_upper(data().bytes);
}

std::vector<ipfs::Byte> const& Multihash::toBuffer() const {
  auto& d = data();
  return d.bytes;
}

bool Multihash::operator==(const Multihash& other) const {
  const auto& a = data();
  const auto& b = other.data();
  if (data_ == other.data_) {
    return true;
  }
  return a.bytes == b.bytes && a.type == b.type;
}

bool Multihash::operator!=(const Multihash& other) const {
  return !(this->operator==(other));
}

bool Multihash::operator<(const class libp2p::multi::Multihash& other) const {
  const auto& a = data();
  const auto& b = other.data();
  if (a.type == b.type) {
    return a.bytes < b.bytes;
  }
  return a.type < b.type;
}

}  // namespace libp2p::multi
