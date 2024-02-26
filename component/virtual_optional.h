#ifndef IPFS_CHROMIUM_VIRTUAL_OPTIONAL_H
#define IPFS_CHROMIUM_VIRTUAL_OPTIONAL_H

#include <base/check_version_internal.h>

#if __has_include(<absl/types/optional.h>)
#include <absl/types/optional.h>
#define HAS_ABSL_OPTIONAL 1
#endif

#if __has_include(<optional>)
#include <optional>
#define HAS_STD_OPTIONAL
#endif

namespace ipfs {

#ifndef HAS_ABSL_OPTIONAL
// If you don't even have access to absl, strong indication to use std
template <class T>
using VirtualOptional = std::optional<T>;
#elif !defined(HAS_STD_OPTIONAL)
// If you don't even have access to std, strong indication to use absl
template <class T>
using VirtualOptional = absl::optional<T>;
#elif BASE_CHECK_VERSION_INTERNAL < 124
// In older Chromium versions,
// these virtual interfaces are defined with absl::optional args
template <class T>
using VirtualOptional = absl::optional<T>;
#else
template <class T>
using VirtualOptional = std::optional<T>;
#endif

}  // namespace ipfs

#endif  // IPFS_CHROMIUM_VIRTUAL_OPTIONAL_H
