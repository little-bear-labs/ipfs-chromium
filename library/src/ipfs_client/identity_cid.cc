#include <ipfs_client/identity_cid.h>

#include <log_macros.h>

namespace Self = ipfs::id_cid;

auto Self::forText(std::string_view txt) -> Cid {
  txt = txt.substr(0UL, MaximumHashLength);
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