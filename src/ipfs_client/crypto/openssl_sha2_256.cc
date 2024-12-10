#include "openssl_sha2_256.h"
#include "vocab/byte_view.h"
#include <optional>
#include <vector>
#include "vocab/byte.h"

using Self = ipfs::crypto::OpensslSha2_256;

#include "log_macros.h"

#if HAS_OPENSSL_SHA

#include <openssl/sha.h>

Self::~OpensslSha2_256() noexcept = default;
auto Self::hash(ipfs::ByteView data) -> std::optional<std::vector<Byte>> {
  SHA256_CTX ctx;
  if (1 != SHA256_Init(&ctx)) {
    LOG(ERROR) << "Failed to initialize SHA256";
    return std::nullopt;
  }
  if (1 != SHA256_Update(&ctx, data.data(), data.size())) {
    LOG(ERROR) << "Failure injesting data into SHA256.";
    return {};
  }
  std::vector<Byte> result(SHA256_DIGEST_LENGTH, Byte{});
  auto *p = reinterpret_cast<unsigned char*>(result.data());
  if (1 == SHA256_Final(p, &ctx)) {
    return result;
  } else {
    LOG(ERROR) << "Error calculating sha2-256 hash.";
    return std::nullopt;
  }
}

#endif
