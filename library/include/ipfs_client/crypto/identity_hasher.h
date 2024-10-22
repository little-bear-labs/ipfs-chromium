#ifndef IDENTITY_HASHER_H
#define IDENTITY_HASHER_H

#include "hasher.h"

namespace ipfs::crypto {
class IdentityHasher final : public Hasher {
    std::optional<std::vector<Byte>> hash(ByteView bytes) {
        return std::vector<Byte>(bytes.begin(), bytes.end());
    }
};
}

#endif // IDENTITY_HASHER_H
