#include <ipfs_client/identity_cid.h>

#include <log_macros.h>
#include <string_view>
#include "ipfs_client/cid.h"
#include "ipfs_client/multi_hash.h"
#include "vocab/byte.h"
#include "vocab/byte_view.h"
#include "ipfs_client/multicodec.h"

namespace Self = ipfs::id_cid;

auto Self::forText(std::string_view txt) -> Cid {
  txt = txt.substr(0UL, MaximumHashLength);
  const auto *p = reinterpret_cast<Byte const*>(txt.data());
  auto b = ByteView{p, txt.size()};
  MultiHash const mh(HashType::IDENTITY, b);
  if (mh.valid()) {
    return Cid{MultiCodec::RAW, mh};
  } else {
    LOG(FATAL)
        << "We really shouldn't be able to fail to 'hash' using identity.";
#ifdef __clang__
#pragma clang diagnostic ignored "-Wunreachable-code-return"
#endif
    return forText("Unreachable");
  }
}