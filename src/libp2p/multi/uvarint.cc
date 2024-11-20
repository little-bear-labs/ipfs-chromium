/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <libp2p/multi/uvarint.hpp>
#include "vocab/byte.h"
#include "vocab/byte_view.h"
#include <optional>
#include <vector>

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

auto UVarint::create(ipfs::ByteView varint_bytes) -> std::optional<UVarint> {
  size_t const size = calculateSize(varint_bytes);
  if (size > 0 && size <= varint_bytes.size()) {
    return UVarint{varint_bytes, size};
  }
  return {};
}

auto UVarint::toUInt64() const -> uint64_t {
  uint64_t res = 0;
  size_t index = 0;
  for (const auto& byte : bytes_) {
    res += static_cast<uint64_t>((byte & ipfs::Byte{0x7f})) << index;
    index += 7;
  }
  return res;
}

auto UVarint::toBytes() const -> ipfs::ByteView {
  return ipfs::ByteView{bytes_.data(), bytes_.size()};
}

auto UVarint::toVector() const -> std::vector<ipfs::Byte> const& {
  return bytes_;
}

auto UVarint::size() const -> size_t {
  return bytes_.size();
}

auto UVarint::operator=(UVarint const& rhs) -> UVarint& = default;
auto UVarint::operator=(uint64_t n) -> UVarint& {
  *this = UVarint(n);
  return *this;
}

auto UVarint::operator==(const UVarint& r) const -> bool {
  return std::equal(bytes_.begin(), bytes_.end(), r.bytes_.begin(),
                    r.bytes_.end());
}

auto UVarint::operator!=(const UVarint& r) const -> bool {
  return !(*this == r);
}

auto UVarint::operator<(const UVarint& r) const -> bool {
  return toUInt64() < r.toUInt64();
}

auto UVarint::calculateSize(ipfs::ByteView varint_bytes) -> size_t {
  size_t size = 0;
  size_t shift = 0;
  constexpr size_t capacity = sizeof(uint64_t) * 8;
  bool last_byte_found = false;
  for (const auto& byte : varint_bytes) {
    ++size;
    std::uint_least64_t const slice = to_integer(byte) & 0x7f;
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
