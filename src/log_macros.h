#ifndef IPFS_LOG_MACROS_H_
#define IPFS_LOG_MACROS_H_

#include <string_view>

#if __has_include("base/logging.h") //In Chromium

#include "base/logging.h"
#include "base/check_op.h"

#elif __has_include(<loguru.hpp>)

#undef LOGURU_WITH_STREAMS
#define LOGURU_WITH_STREAMS 1
#include <loguru.hpp>

#define DCHECK(X) DCHECK_S( X )
#define DCHECK_EQ(X, Y) DCHECK_EQ_S( X , Y )
#define LOG(X) LOG_S( X )
#define VLOG(X) VLOG_S( X )

#else

#error "Provide logging."

#endif //Chromium in-tree check

#define L_VAR(X) LOG(INFO) << "VAR " << #X << "='" << (X) << '\'';

#endif  // IPFS_LOG_MACROS_H_
