#ifndef IPFS_LOG_MACROS_H_
#define IPFS_LOG_MACROS_H_

#include <string_view>

#if __has_include("base/logging.h") //In Chromium

#include "base/logging.h"
#include "base/check_op.h"

#else // Not in Chromium

#include <ipfs_client/logger.h>

#include <google/protobuf/stubs/logging.h>

#define DCHECK_EQ GOOGLE_DCHECK_EQ
#define DCHECK_GT GOOGLE_DCHECK_GT
#define DCHECK GOOGLE_DCHECK
#define LOG GOOGLE_LOG

#define VLOG(X)                                       \
  ::google::protobuf::internal::LogFinisher() =      \
      ::google::protobuf::internal::LogMessage(       \
          static_cast<::google::protobuf::LogLevel>(  \
              ::google::protobuf::LOGLEVEL_INFO - ( X ) ), \
          __FILE__, __LINE__)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
namespace {
static bool const is_logging_initialized = ::ipfs::log::IsInitialized();
} // namespace
#pragma GCC diagnostic pop

#endif //Chromium in-tree check

#define L_VAR(X) LOG(INFO) << "VAR " << #X << "='" << (X) << '\'';

inline auto starts_with(std::string_view full_text, std::string_view prefix) -> bool {
  if (prefix.size() > full_text.size()) {
    return false;
  }
  return full_text.substr(0UL, prefix.size()) == prefix;
}
inline auto ends_with(std::string_view full_text, std::string_view suffix) -> bool {
  if (suffix.size() > full_text.size()) {
    return false;
  }
  return full_text.substr(full_text.size() - suffix.size()) == suffix;
}

#endif  // IPFS_LOG_MACROS_H_
