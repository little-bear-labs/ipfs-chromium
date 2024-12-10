#include "car.h"

#include <ipfs_client/client.h>
#include "ipfs_client/ctx/cbor_parser.h"

#include <libp2p/multi/uvarint.hpp>
#include <vocab/byte.h>
#include "vocab/byte_view.h"

#include "log_macros.h"

#include <cassert>
#include <cstdint>

#include <numeric>
#include <utility>
#include <optional>

using Self = ipfs::Car;
using Byte = ipfs::Byte;
using ByteView = ipfs::ByteView;
using VarInt = libp2p::multi::UVarint;

namespace {
auto ReadHeader(ByteView& /*bytes*/, ipfs::ctx::CborParser& /*cbor_parser*/) -> short;
auto GetV1PayloadPos(ByteView /*bytes*/) -> std::pair<std::uint64_t, std::uint64_t>;
}  // namespace

Self::Car(ByteView bytes, ctx::CborParser& cbor_parser) {
  auto after_header = bytes;
  auto version = ReadHeader(after_header, cbor_parser);
  switch (version) {
    case 0:
      VLOG(2) << "Problem parsing CAR header.";
      break;
    case 1:
      data_ = after_header;
      break;
    case 2: {
      auto [off, siz] = GetV1PayloadPos(after_header);
      LOG(INFO) << "CARv2 carries a payload of " << siz << "B @ " << off;
      if (bytes.size() - after_header.size() > off) {
        LOG(ERROR) << "CARv2 payload is supposedly offset into the V1 header";
        break;
      }
      if (siz > after_header.size()) {
        LOG(ERROR) << "Payload size indicated by V1 header too large.";
        break;
      }
      data_ = bytes.subspan(off, siz);
      ReadHeader(data_, cbor_parser);
      break;
    }
    default:
      LOG(ERROR) << "Unsupported CAR format version " << version;
  }
}
auto Self::NextBlock() -> std::optional<Block> {
  auto len = VarInt::create(data_);
  if (!len) {
    return std::nullopt;
  }
  data_ = data_.subspan(len->size());
  if (len->toUInt64() > data_.size()) {
    LOG(ERROR) << "Length prefix claims cid+block is " << len->toUInt64()
               << " bytes, but I only have " << data_.size()
               << " bytes left in the CAR payload.";
    data_ = {};
    return std::nullopt;
  }
  Block blk;
  blk.bytes = data_.subspan(0U, len->toUInt64());
  data_ = data_.subspan(len->toUInt64());
  if (blk.cid.ReadStart(blk.bytes)) {
    return blk;
  }
  return std::nullopt;
}

namespace {
// https://ipld.io/specs/transport/car/carv2/
auto ReadHeader(ByteView& bytes, ipfs::ctx::CborParser& cbor_parser) -> short {
  auto header_len = VarInt::create(bytes);
  if (!header_len ||
      header_len->toUInt64() + header_len->size() > bytes.size()) {
    return 0;
  }
  bytes = bytes.subspan(header_len->size());
  auto header_bytes = bytes.subspan(0UL, header_len->toUInt64());
  auto header = cbor_parser.Parse(header_bytes);
  if (!header) {
    return 0;
  }
  auto version_node = header->at("version");
  if (!version_node) {
    return 0;
  }
  auto version = version_node->as_unsigned();
  if (version) {
    bytes = bytes.subspan(header_len->toUInt64());
    assert(version.value() < 0x8000U);
    return static_cast<short>(version.value());
  }
  return 0;
}
// NOLINTBEGIN(readability-magic-numbers)
auto read_le_u64(ByteView bytes, unsigned& off) -> std::uint64_t {
  auto b = bytes.subspan(off, off + 8);
  off += 8U;
  auto shift_in = [](std::uint64_t i, Byte y) {
    return (i << 8) | static_cast<unsigned>(y);
  };
  return std::accumulate(b.rbegin(), b.rend(), 0UL, shift_in);
}
auto GetV1PayloadPos(ByteView bytes) -> std::pair<std::uint64_t, std::uint64_t> {
  // Following the 11 byte pragma, the CARv2 [header] is a fixed-length sequence
  // of 40 bytes, broken into the following sections:
  if (bytes.size() < 40) {
    return {};
  }

  // Characteristics: A 128-bit (16-byte) bitfield used to describe certain
  // features of the enclosed data.
  auto reading_off = 16U;

  // Data offset: A 64-bit (8-byte) unsigned
  // little-endian integer indicating the byte-offset from the beginning of the
  // CARv2 [pragma] to the first byte of the CARv1 data payload.
  auto data_offset = read_le_u64(bytes, reading_off);

  // Data size: A 64-bit
  // (8-byte) unsigned little-endian integer indicating the byte-length of the
  // CARv1 data payload.
  auto data_size = read_le_u64(bytes, reading_off);

  // Index offset: A 64-bit (8-byte) unsigned little-endian
  // integer indicating the byte-offset from the beginning of the CARv2 to the
  // first byte of the index payload. This value may be 0 to indicate the
  // absence of index data.
  reading_off += 8;  // Ignoring index and therefore index offset

  assert(reading_off == 40UL);

  return {data_offset, data_size};
}
// NOLINTEND(readability-magic-numbers)

}  // namespace

