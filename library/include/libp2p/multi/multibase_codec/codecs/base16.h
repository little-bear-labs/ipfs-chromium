#ifndef IPFS_BASE32_H_
#define IPFS_BASE32_H_

#include "base_error.hpp"

#include <vocab/byte_view.h>
#include <vocab/expected.h>

#include <libp2p/common/types.hpp>

#include <string>

namespace ipfs::base16 {
std::string encodeLower(ByteView bytes);
std::string encodeUpper(ByteView bytes);

using libp2p::common::ByteArray;
using libp2p::multi::detail::BaseError;
using Decoded = ipfs::expected<ByteArray, BaseError>;
Decoded decode(std::string_view string);

}  // namespace ipfs::base16

#endif  // IPFS_BASE32_H_
