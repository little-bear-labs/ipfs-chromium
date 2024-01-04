#include "car.h"

#include <ipfs_client/context_api.h>

#include <libp2p/multi/uvarint.hpp>

#include "log_macros.h"

#include <numeric>

using Self = ipfs::Car;
using Byte = ipfs::Byte;
using ByteView = ipfs::ByteView;
using VarInt = libp2p::multi::UVarint;

namespace {
short ReadHeader(ByteView&, ipfs::ContextApi const&);
std::pair<std::uint64_t, std::uint64_t> GetV1PayloadPos(ByteView);
}  // namespace

Self::Car(ByteView bytes, ContextApi const& api) {
  auto after_header = bytes;
  auto version = ReadHeader(after_header, api);
  switch (version) {
    case 0:
      LOG(ERROR) << "Problem parsing CAR header.";
      break;
    case 1:
      data_ = after_header;
      break;
    case 2: {
      auto [off, siz] = GetV1PayloadPos(after_header);
      LOG(INFO) << "CARv2 carries a payload of " << siz << "B @ " << off;
      // TODO validate off and siz are sane, e.g. not pointing back into pragma
      // or whatever
      data_ = bytes.subspan(off, siz);
      ReadHeader(data_, api);
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
  Block rv;
  rv.bytes = data_.subspan(0U, len->toUInt64());
  data_ = data_.subspan(len->toUInt64());
  if (rv.cid.ReadStart(rv.bytes)) {
    return rv;
  }
  return std::nullopt;
}

namespace {
// https://ipld.io/specs/transport/car/carv2/
short ReadHeader(ByteView& bytes, ipfs::ContextApi const& api) {
  auto header_len = VarInt::create(bytes);
  if (!header_len ||
      header_len->toUInt64() + header_len->size() > bytes.size()) {
    return 0;
  }
  bytes = bytes.subspan(header_len->size());
  auto header_bytes = bytes.subspan(0UL, header_len->toUInt64());
  auto header = api.ParseCbor(header_bytes);
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
    return version.value();
  }
  return 0;
}
std::uint64_t read_le_u64(ByteView bytes, unsigned& off) {
  auto b = bytes.subspan(off, off + 8);
  off += 8U;
  auto shift_in = [](std::uint64_t i, Byte y) {
    return (i << 8) | static_cast<unsigned>(y);
  };
  return std::accumulate(b.rbegin(), b.rend(), 0UL, shift_in);
}
std::pair<std::uint64_t, std::uint64_t> GetV1PayloadPos(ByteView bytes) {
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
}  // namespace