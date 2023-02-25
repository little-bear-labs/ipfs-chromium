#pragma once

// TODO this file should go away in favor of libp2p/multi/content_identifier*

#include <libp2p/multi/multicodec_type.hpp>

#include <string_view>

namespace cid {
using Multicodec = libp2p::multi::MulticodecType::Code;
namespace bin {
constexpr bool is_v0(std::string_view bytes) {
  // If it's 34 bytes long... leading bytes [0x12, 0x20, ...], it's a CIDv0
  return bytes.size() == 34UL && bytes[0] == 0x12 && bytes[1] == 0x20;
}

constexpr Multicodec get_multicodec(std::string_view bytes) {
  if (is_v0(bytes)) {
    return Multicodec::DAG_PB;
  }
  if (bytes.size() < 3) {
    //    throw std::invalid_argument{"Only " + std::to_string(bytes.size()) +
    //                                " bytes in a binary CID?"};
    return Multicodec::SHA3_384;
  }
  if (bytes.front() != 0x01) {
    //    throw std::invalid_argument{
    //        std::to_string(static_cast<short>(bytes.front())) +
    //        " is not a supported CID version"};
    return Multicodec::SHA3_224;
  }
  // This is NOT how varint works.
  return static_cast<Multicodec>(bytes[1]);
}
}  // namespace bin
namespace mb {
std::string to_bin(std::string_view mb_encoded_cid);
}  // namespace mb
}  // namespace cid