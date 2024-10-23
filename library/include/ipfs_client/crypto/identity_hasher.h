#ifndef IDENTITY_HASHER_H
#define IDENTITY_HASHER_H

#include "hasher.h"

namespace ipfs::crypto {
/*! The simplest conceivable "hasher" beyond a null object.
 *  Arguably not a hasher, but there's definitely no collisions.
 */
class IdentityHasher final : public Hasher {

    /*! @param bytes The bytes to 'hash'
     *  @return A deep copy of the bytes parameter
     */
    std::optional<std::vector<Byte>> hash(ByteView bytes) {
        return std::vector<Byte>(bytes.begin(), bytes.end());
    }
};
}

#endif // IDENTITY_HASHER_H
