#include <ipfs_client/cid_util.h>

// TODO delete this file in favor of libp2p strong types

#include <vocab/log_macros.h>

#include <libp2p/multi/multibase_codec/codecs/base32.hpp>
#include <libp2p/multi/multibase_codec/codecs/base58.hpp>

#include <cassert>

std::string cid::mb::to_bin(std::string_view mb) {
  // If it is 46 characters long and starts with Qm..., it's a CIDv0.
  //  Decode it as base58btc and continue to step 2.
  if (mb.size() == 46 && mb[0] == 'Q' && mb[1] == 'm') {
    return libp2p::multi::detail::decodeBase58(mb.substr(4)).value();
  }
  assert(mb.size() > 0UL);
  // Otherwise, decode it according to the multibase spec
  switch (mb.front()) {
    case 'b':
      return libp2p::multi::detail::decodeBase32Lower(mb.substr(1)).value();
    default:
      L_DIE("Unsupported multibase prefix: " << mb.at(0));
  }
  std::abort();
}
