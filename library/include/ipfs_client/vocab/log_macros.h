#ifndef IPFS_LOG_MACROS_H_
#define IPFS_LOG_MACROS_H_

#ifndef IPFS_CLIENT_LOGGING_ENABLED
#define IPFS_CLIENT_LOGGING_ENABLED 1
#endif

#if !IPFS_CLIENT_LOGGING_ENABLED

#define L_INF(...)
#define L_WRN(...)
#define L_ERR(...)
#define L_DIE(...) std::abort();

// TODO real log library with levels and such,
//    that works with stream operators (not fmt like spdlog & quill),
//    outside of Chromium
// TODO log to checkable memory for tests?

#elif __has_include("base/logging.h")

#include "base/logging.h"
#define L_INF(...) \
  { LOG(INFO) << __VA_ARGS__; }
#define L_WRN(...) \
  { LOG(WARNING) << __VA_ARGS__; }
#define L_ERR(...) \
  { LOG(ERROR) << __VA_ARGS__; }
#define L_DIE(...) \
  { LOG(FATAL) << __VA_ARGS__; }

#else

#include <cstdlib>
#include <iostream>

#define L_INF(...)                                                           \
  {                                                                          \
    std::clog << "INF " << __FILE__ << ':' << __LINE__ << ' ' << __VA_ARGS__ \
              << '\n';                                                       \
  }
#define L_WRN(...)                                                           \
  {                                                                          \
    std::clog << "WRN " << __FILE__ << ':' << __LINE__ << ' ' << __VA_ARGS__ \
              << '\n';                                                       \
  }
#define L_ERR(...)                                                           \
  {                                                                          \
    std::clog << "ERR " << __FILE__ << ':' << __LINE__ << ' ' << __VA_ARGS__ \
              << '\n';                                                       \
  }
#define L_DIE(...)                                                        \
  {                                                                       \
    std::clog << "Fatal error! Aborting! " << __FILE__ << ':' << __LINE__ \
              << ' ' << __VA_ARGS__ << std::endl;                         \
    std::abort();                                                         \
  }

#endif

#endif  // IPFS_LOG_MACROS_H_
