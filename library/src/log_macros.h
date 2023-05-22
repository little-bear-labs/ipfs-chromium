#ifndef IPFS_LOG_MACROS_H_
#define IPFS_LOG_MACROS_H_


#if __has_include("base/logging.h") //In Chromium

#include "base/logging.h"
#include "base/check_op.h"

#else // Not in Chromium

#include <ipfs_client/logger.h>

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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
namespace {
static bool is_logging_initialized = ::ipfs::log::IsInitialized();
}
#pragma GCC diagnostic pop

#endif //Chromium in-tree check

#define L_VAR(X) LOG(INFO) << "VAR " << #X << "='" << (X) << '\'';

#endif  // IPFS_LOG_MACROS_H_
