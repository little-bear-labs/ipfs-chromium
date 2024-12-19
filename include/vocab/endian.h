#ifndef IPFS_ENDIAN_H_
#define IPFS_ENDIAN_H_

#if __has_include(<endian.h>)
#include <endian.h>
#endif
#if __has_include(<machine/endian.h>)
#include <machine/endian.h>
#endif

#ifdef htobe64
// Good
#elif __has_include(<absl/base/internal/endian.h>)
#include <absl/base/internal/endian.h>
#define htobe64 absl::ghtonll
#elif __has_include(<boost/endian/conversion.hpp>)
#include <boost/endian/conversion.hpp>
#define htobe64 native_to_big
#endif

#endif  // IPFS_ENDIAN_H_
