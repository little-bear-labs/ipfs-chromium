#include <ipfs_client/identity_cid.h>

#include <libp2p/multi/multihash.hpp>

#include <log_macros.h>

namespace Self = ipfs::id_cid;
namespace m = libp2p::multi;

auto Self::forText(std::string_view txt) -> Cid {
  txt = txt.substr(0UL, m::Multihash::kMaxHashLength);
  auto p = reinterpret_cast<Byte const*>(txt.data());
  auto b = ByteView{p, txt.size()};
  auto mh = m::Multihash::create(m::HashType::identity, b);
  if (mh.has_value()) {
    return Cid{Cid::Version::V1, m::MulticodecType::Code::RAW, mh.value()};
  } else {
    LOG(FATAL)
        << "We really shouldn't be able to fail to 'hash' using identity "
        << static_cast<int>(mh.error());
    return forText("Unreachable");
  }
}