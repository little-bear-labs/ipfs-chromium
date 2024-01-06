#include <ipfs_client/context_api.h>

#include "crypto/openssl_sha2_256.h"

using Self = ipfs::ContextApi;

Self::ContextApi() {
#if HAS_OPENSSL_SHA
  hashers_.emplace(HashType::SHA2_256,
                   std::make_unique<crypto::OpensslSha2_256>());
#endif
}

auto Self::Hash(HashType ht, ByteView data)
    -> std::optional<std::vector<Byte>> {
  auto it = hashers_.find(ht);
  if (hashers_.end() == it || !(it->second)) {
    return std::nullopt;
  }
  return it->second->hash(data);
}

void Self::SetGatewayRate(std::string_view, unsigned int) {}
void Self::AddGateway(std::string_view gw) {
  SetGatewayRate(gw, 60U);
}