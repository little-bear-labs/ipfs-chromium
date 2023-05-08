#ifndef IPFS_LOG_MACROS_H_
#define IPFS_LOG_MACROS_H_

#ifndef IPFS_CLIENT_LOGGING_ENABLED
#define IPFS_CLIENT_LOGGING_ENABLED 1
#endif

#if __has_include("base/logging.h")

#include "base/logging.h"

#else

#include <google/protobuf/stubs/logging.h>

#define DCHECK_EQ GOOGLE_DCHECK_EQ
#define DCHECK GOOGLE_DCHECK
#define LOG GOOGLE_LOG
// TODO
#define VLOG(X) GOOGLE_LOG(INFO - X)

#endif

#define L_VAR(X) LOG(INFO) << "VAR " << #X << "='" << (X) << '\'';

#endif  // IPFS_LOG_MACROS_H_
