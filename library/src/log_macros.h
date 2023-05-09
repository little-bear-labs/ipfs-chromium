#ifndef IPFS_LOG_MACROS_H_
#define IPFS_LOG_MACROS_H_

#ifndef IPFS_CLIENT_LOGGING_ENABLED
#define IPFS_CLIENT_LOGGING_ENABLED 1
#endif

#if __has_include("base/logging.h")

#include "base/logging.h"
#include "base/check_op.h"

#else

#include <google/protobuf/stubs/logging.h>

#define DCHECK_EQ GOOGLE_DCHECK_EQ
#define DCHECK GOOGLE_DCHECK
#define LOG GOOGLE_LOG
// TODO
#define VLOG(X)                                       \
  ::google::protobuf::internal::LogFinisher() =       \
      ::google::protobuf::internal::LogMessage(       \
          static_cast<::google::protobuf::LogLevel>(  \
              ::google::protobuf::LOGLEVEL_INFO - X), \
          __FILE__, __LINE__)

#endif

#define L_VAR(X) LOG(INFO) << "VAR " << #X << "='" << (X) << '\'';

#endif  // IPFS_LOG_MACROS_H_
