#ifndef IPFS_I128_H_
#define IPFS_I128_H_

#if __has_include(<absl/numeric/int128.h>)
#include <absl/numeric/int128.h>
namespace ipfs {
using Int_128 = absl::int128;
}
#else
namespace ipfs {
// TODO Check if available, if not use boost multiprecision
using Int_128 = __int128;
}  // namespace ipfs
#endif

#endif  // IPFS_I128_H_
