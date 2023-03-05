#ifndef CHROMIUM_IPFS_BYTE_VIEW_H
#define CHROMIUM_IPFS_BYTE_VIEW_H

#include "byte.h"
#include "span.h"

namespace ipfs {
using ByteView = span<ipfs::Byte const>;
}

#endif  // CHROMIUM_IPFS_BYTE_VIEW_H
