#include <ipfs_client/cid.h>

#include <ipfs_client/multi_base.h>
#include <libp2p/multi/uvarint.hpp>

#include "log_macros.h"

using Self = ipfs::Cid;
using VarInt = libp2p::multi::UVarint;

Self::Cid(ipfs::MultiCodec cdc, ipfs::MultiHash hsh)
    : codec_{cdc}, hash_{hsh} {}

Self::Cid(ipfs::ByteView bytes) {
  ReadStart(bytes);
}

Self::Cid(std::string_view s) {
  if (s.size() == 46 && s[0] == 'Q' && s[1] == 'm') {
    auto bytes = mb::Codec::Get(mb::Code::BASE58_BTC)->decode(s);
    auto view = ByteView{bytes};
    ReadStart(view);
  } else if (auto bytes = mb::decode(s)) {
    if (bytes->size() > 4) {
      auto view = ByteView{bytes.value()};
      ReadStart(view);
    }
  }
}

bool Self::ReadStart(ByteView& bytes) {
  if (bytes.size() >= 34 && bytes[0] == ipfs::Byte{0x12} &&
      bytes[1] == ipfs::Byte{0x20}) {
    hash_ = MultiHash{bytes};
    codec_ = hash_.valid() ? MultiCodec::DAG_PB : MultiCodec::INVALID;
    bytes = bytes.subspan(34);
    return true;
  }
  auto version = VarInt::create(bytes);
  if (!version) {
    return false;
  }
  if (version->toUInt64() != 1U) {
    LOG(ERROR) << "CID version " << version->toUInt64() << " not supported.";
    return false;
  }
  bytes = bytes.subspan(version->size());
  auto codec = VarInt::create(bytes);
  if (!codec) {
    return false;
  }
  auto cdc = static_cast<MultiCodec>(codec->toUInt64());
  codec_ = Validate(cdc);
  bytes = bytes.subspan(codec->size());
  return hash_.ReadPrefix(bytes);
}

bool Self::valid() const {
  return codec_ != MultiCodec::INVALID && hash_.valid();
}

auto Self::hash() const -> ByteView {
  return hash_.digest();
}
auto Self::hash_type() const -> HashType {
  return multi_hash().type();
}

std::string Self::to_string() const {
  std::vector<Byte> binary;
  auto append_varint = [&binary](auto x) {
    auto i = static_cast<std::uint64_t>(x);
    VarInt v{i};
    auto b = v.toBytes();
    binary.insert(binary.end(), b.begin(), b.end());
  };
  append_varint(1);  // CID version 1
  append_varint(codec());
  append_varint(hash_type());
  append_varint(hash().size());
  auto h = hash();
  binary.insert(binary.end(), h.begin(), h.end());
  return mb::encode(mb::Code::BASE32_LOWER, binary);
}
