/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_VARINT_HPP
#define LIBP2P_VARINT_HPP

#include "vocab/byte_view.h"

#include <cstdint>

#include <optional>
#include <string>
#include <vector>

namespace libp2p::multi {

/**
 * @class Encodes and decodes unsigned integers into and from
 * variable-length byte arrays using LEB128 algorithm.
 */
class UVarint {
 public:
  /**
   * Constructs a varint from an unsigned integer 'number'
   * @param number
   */
  explicit UVarint(uint64_t number);

  /**
   * Constructs a varint from an array of raw bytes, which are
   * meant to be an already encoded unsigned varint
   * @param varint_bytes an array of bytes representing an unsigned varint
   */
  explicit UVarint(ipfs::ByteView varint_bytes);

  /**
   * Constructs a varint from an array of raw bytes, which beginning may or
   * may not be an encoded varint
   * @param varint_bytes an array of bytes, possibly representing an unsigned
   * varint
   */
  static std::optional<UVarint> create(ipfs::ByteView varint_bytes);

  /**
   * Converts a varint back to a usual unsigned integer.
   * @return an integer previously encoded to the varint
   */
  uint64_t toUInt64() const;

  /**
   * @return an array view to raw bytes of the stored varint
   */
  ipfs::ByteView toBytes() const;

  std::vector<ipfs::Byte> const& toVector() const;

  std::string toHex() const;

  /**
   * Assigns the varint to an unsigned integer, encoding the latter
   * @param n the integer to encode and store
   * @return this varint
   */
  UVarint& operator=(uint64_t n);

  bool operator==(const UVarint& r) const;
  bool operator!=(const UVarint& r) const;
  bool operator<(const UVarint& r) const;

  /**
   * @return the number of bytes currently stored in a varint
   */
  size_t size() const;

  /**
   * @param varint_bytes an array with a raw byte representation of a varint
   * @return the size of the varint stored in the array, if its content is a
   * valid varint. Otherwise, the result is undefined
   */
  static size_t calculateSize(ipfs::ByteView varint_bytes);

  UVarint() = delete;
  UVarint(UVarint const&);
  UVarint& operator=(UVarint const&);
  ~UVarint() noexcept;

 private:
  /// private ctor for unsafe creation
  UVarint(ipfs::ByteView varint_bytes, size_t varint_size);

  std::vector<ipfs::Byte> bytes_{};
};

}  // namespace libp2p::multi

#endif  // LIBP2P_VARINT_HPP
