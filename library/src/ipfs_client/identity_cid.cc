#include <ipfs_client/identity_cid.h>

#include <libp2p/multi/multihash.hpp>

#include <log_macros.h>

namespace Self = ipfs::id_cid;
namespace m = libp2p::multi;

auto Self::forText(std::string_view txt) -> Cid {
  txt = txt.substr(0UL, m::Multihash::kMaxHashLength);
  auto p = reinterpret_cast<Byte const*>(txt.data());
  auto b = ByteView{p, txt.size()};
  MultiHash mh(HashType::IDENTITY, b);
  if (mh.valid()) {
    return Cid{MultiCodec::RAW, mh};
  } else {
    LOG(FATAL)
        << "We really shouldn't be able to fail to 'hash' using identity.";
    return forText("Unreachable");
  }
}