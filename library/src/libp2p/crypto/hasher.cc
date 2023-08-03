#include <libp2p/crypto/hasher.hpp>
#include <libp2p/crypto/sha/sha256.hpp>

#include "log_macros.h"

std::unique_ptr<libp2p::crypto::Hasher> libp2p::crypto::CreateHasher(
    multi::HashType algo) {
  switch (algo) {
    case multi::HashType::sha256:
      return std::make_unique<Sha256>();
    default:
      LOG(FATAL) << "Unimplemented hash type: "
                 << static_cast<std::uint16_t>(algo);
  }
  return {};
}
