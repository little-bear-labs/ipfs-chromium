#include <ipfs_client/logger.h>

#include <google/protobuf/stubs/logging.h>

#include <iostream>

namespace log = ipfs::log;

namespace {
log::Level current_level = log::Level::WARN;
log::Handler current_handler = nullptr;

void CheckLevel(google::protobuf::LogLevel lv,
                char const* f,
                int l,
                std::string const& m) {
  auto lev = static_cast<int>(lv);
  if (lev < static_cast<int>(current_level)) {
    return;
  }
  if (!current_handler) {
    return;
  }
  current_handler(m, f, l, static_cast<log::Level>(lev));
}

}  // namespace

void log::SetLevel(Level lev) {
  current_level = lev;
}

void log::SetHandler(Handler h) {
  current_handler = h;
  google::protobuf::SetLogHandler(&CheckLevel);
}

void log::DefaultHandler(std::string const& message,
                         char const* source_file,
                         int source_line,
                         Level lev) {
  std::clog << source_file << ':'
            << source_line << ": "
            << LevelDescriptor(lev) << ": "
            << message << '\n';
  if (lev == Level::FATAL) {
    std::abort();
  }
}

std::string_view log::LevelDescriptor(Level l) {
  switch (l) {
    case Level::TRACE: return "trace";
    case Level::DEBUG: return "debug";
    case Level::INFO: return "note";//The next 3 are gcc- & clang-inspired
    case Level::WARN: return "warning";
    case Level::ERROR: return "error";
    case Level::FATAL: return " ### FATAL ERROR ### ";
    default:
      return "Unknown log level used: possible corruption?";
  }
}

bool log::IsInitialized() {
  if (current_handler) {
    return true;
  }
  SetHandler(&DefaultHandler);
  return false;
}
