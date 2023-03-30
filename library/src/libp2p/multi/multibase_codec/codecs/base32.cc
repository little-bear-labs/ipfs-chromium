/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * base32 (de)coder implementation as specified by RFC4648.
 *
 * Copyright (c) 2010 Adrien Kunysz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 **/

#include "libp2p/multi/multibase_codec/codecs/base32.hpp"
#include "libp2p/multi/multibase_codec/codecs/base_error.hpp"

#include <absl/types/span.h>

namespace libp2p::multi::detail {
const std::string kUpperBase32Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
const std::string kLowerBase32Alphabet = "abcdefghijklmnopqrstuvwxyz234567";

enum Base32Mode {
  LOWER,
  UPPER,
};

int get_byte(int block) {
  return block * 5 / 8;
}

int get_bit(int block) {
  return 8 - 5 - block * 5 % 8;
}

char encode_char(unsigned char c, Base32Mode mode) {
  if (mode == Base32Mode::UPPER) {
    return kUpperBase32Alphabet[c & 0x1F];  // 0001 1111
  }
  return kLowerBase32Alphabet[c & 0x1F];
}

unsigned char shift_right(uint8_t byte, int8_t offset) {
  if (offset > 0) {
    return byte >> offset;
  }

  return byte << -offset;
}

unsigned char shift_left(uint8_t byte, int8_t offset) {
  return shift_right(byte, -offset);
}

int encode_sequence(ipfs::span<const uint8_t> plain,
                    ipfs::span<char> coded,
                    Base32Mode mode) {
  for (int block = 0; block < 8; block++) {
    int byte = get_byte(block);
    int bit = get_bit(block);

    if (byte >= static_cast<long>(plain.size())) {
      return block;
    }

    unsigned char c = shift_right(plain[byte], bit);

    if (bit < 0 && byte < static_cast<long>(plain.size()) - 1L) {
      c |= shift_right(plain[byte + 1], 8 + bit);
    }
    coded[block] = encode_char(c, mode);
  }
  return 8;
}

std::string encodeBase32(const common::ByteArray& bytes, Base32Mode mode) {
  std::string result;
  if (bytes.size() % 5 == 0) {
    result = std::string(bytes.size() / 5 * 8, ' ');
  } else {
    result = std::string((bytes.size() / 5 + 1) * 8, ' ');
  }

  for (size_t i = 0, j = 0; i < bytes.size(); i += 5, j += 8) {
    int n = encode_sequence(
        ipfs::span<uint8_t const>(reinterpret_cast<const uint8_t*>(&bytes[i]),
                                  std::min<size_t>(bytes.size() - i, 5)),
        ipfs::span<char>(&result[j], 8U), mode);
    if (n < 8) {
      result.erase(result.end() - (8 - n), result.end());
    }
  }

  return result;
}

std::string encodeBase32Upper(const common::ByteArray& bytes) {
  return encodeBase32(bytes, Base32Mode::UPPER);
}

std::string encodeBase32Lower(const common::ByteArray& bytes) {
  return encodeBase32(bytes, Base32Mode::LOWER);
}

int decode_char(unsigned char c, Base32Mode mode) {
  char decoded_ch = -1;

  if (mode == Base32Mode::UPPER) {
    if (c >= 'A' && c <= 'Z') {
      decoded_ch = c - 'A';  // NOLINT
    }
  } else {
    if (c >= 'a' && c <= 'z') {
      decoded_ch = c - 'a';  // NOLINT
    }
  }
  if (c >= '2' && c <= '7') {
    decoded_ch = c - '2' + 26;  // NOLINT
  }

  return decoded_ch;
}

ipfs::expected<int, BaseError> decode_sequence(ipfs::span<const char> coded,
                                               ipfs::span<uint8_t> plain,
                                               Base32Mode mode) {
  plain[0] = 0;
  for (int block = 0; block < 8; block++) {
    int bit = get_bit(block);
    int byte = get_byte(block);

    if (block >= static_cast<long>(coded.size())) {
      return byte;
    }
    int c = decode_char(coded[block], mode);
    if (c < 0) {
      //      return absl::InvalidArgumentError("INVALID_BASE32_INPUT");
      return ipfs::unexpected<BaseError>{BaseError::INVALID_BASE32_INPUT};
    }

    plain[byte] |= shift_left(c, bit);
    if (bit < 0) {
      plain[byte + 1] = shift_left(c, 8 + bit);
    }
  }
  return 5;
}

ipfs::expected<common::ByteArray, BaseError> decodeBase32(
    std::string_view string,
    Base32Mode mode) {
  common::ByteArray result;
  if (string.size() % 8 == 0) {
    result = common::ByteArray(string.size() / 8 * 5, ipfs::Byte{0});
  } else {
    result = common::ByteArray((string.size() / 8 + 1) * 5, ipfs::Byte{0});
  }

  for (size_t i = 0, j = 0; i < string.size(); i += 8, j += 5) {
    auto n = decode_sequence(
        ipfs::span<char const>(&string[i],
                               std::min<size_t>(string.size() - i, 8)),
        ipfs::span<uint8_t>(reinterpret_cast<uint8_t*>(&result[j]), 5U), mode);
    if (!n.has_value()) {
      return ipfs::unexpected<BaseError>{n.error()};
    }
    if (n.value() < 5) {
      result.erase(result.end() - (5 - n.value()), result.end());
    }
  }
  return result;
}

ipfs::expected<common::ByteArray, BaseError> decodeBase32Upper(
    std::string_view string) {
  return decodeBase32(string, Base32Mode::UPPER);
}

ipfs::expected<common::ByteArray, BaseError> decodeBase32Lower(
    std::string_view string) {
  return decodeBase32(string, Base32Mode::LOWER);
}

}  // namespace libp2p::multi::detail
