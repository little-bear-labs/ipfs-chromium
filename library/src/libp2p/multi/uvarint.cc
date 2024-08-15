/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <libp2p/multi/uvarint.hpp>

namespace libp2p::multi {

UVarint::UVarint(UVarint const& rhs)  = default;
UVarint::UVarint(uint64_t number) {
  do {
    auto byte = static_cast<ipfs::Byte>(number) & ipfs::Byte{0x7f};
    number >>= 7;
    if (number != 0) {
      byte |= ipfs::Byte{0x80};
    }
    bytes_.push_back(byte);
  } while (number != 0);
}

UVarint::UVarint(ipfs::ByteView varint_bytes) {
  auto size = calculateSize(varint_bytes);
  if (size <= varint_bytes.size()) {
    bytes_.assign(varint_bytes.begin(), varint_bytes.begin() + size);
  }
}

UVarint::UVarint(ipfs::ByteView varint_bytes, size_t varint_size)
    : bytes_(varint_bytes.begin(), varint_bytes.begin() + varint_size) {}

std::optional<UVarint> UVarint::create(ipfs::ByteView varint_bytes) {
  size_t size = calculateSize(varint_bytes);
  if (size > 0 && size <= varint_bytes.size()) {
    return UVarint{varint_bytes, size};
  }
  return {};
}

uint64_t UVarint::toUInt64() const {
  uint64_t res = 0;
  size_t index = 0;
  for (const auto& byte : bytes_) {
    res += static_cast<uint64_t>((byte & ipfs::Byte{0x7f})) << index;
    index += 7;
  }
  return res;
}

ipfs::ByteView UVarint::toBytes() const {
  return ipfs::ByteView{bytes_.data(), bytes_.size()};
}

std::vector<ipfs::Byte> const& UVarint::toVector() const {
  return bytes_;
}

size_t UVarint::size() const {
  return bytes_.size();
}

UVarint& UVarint::operator=(UVarint const& rhs) = default;
UVarint& UVarint::operator=(uint64_t n) {
  *this = UVarint(n);
  return *this;
}

bool UVarint::operator==(const UVarint& r) const {
  return std::equal(bytes_.begin(), bytes_.end(), r.bytes_.begin(),
                    r.bytes_.end());
}

bool UVarint::operator!=(const UVarint& r) const {
  return !(*this == r);
}

bool UVarint::operator<(const UVarint& r) const {
  return toUInt64() < r.toUInt64();
}

size_t UVarint::calculateSize(ipfs::ByteView varint_bytes) {
  size_t size = 0;
  size_t shift = 0;
  constexpr size_t capacity = sizeof(uint64_t) * 8;
  bool last_byte_found = false;
  for (const auto& byte : varint_bytes) {
    ++size;
    std::uint_least64_t slice = to_integer(byte) & 0x7f;
    if (shift >= capacity || ((slice << shift) >> shift) != slice) {
      size = 0;
      break;
    }
    if ((byte & ipfs::Byte{0x80}) == ipfs::Byte{0}) {
      last_byte_found = true;
      break;
    }
    shift += 7;
  }
  return last_byte_found ? size : 0;
}

UVarint::~UVarint() noexcept = default;

}  // namespace libp2p::multi
