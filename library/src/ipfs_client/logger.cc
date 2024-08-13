#include <ipfs_client/logger.h>

#include <google/protobuf/stubs/logging.h>

#include <iostream>

namespace lg = ipfs::log;

namespace {
lg::Level current_level = lg::Level::Warn;
lg::Handler current_handler = nullptr;

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
  current_handler(m, f, l, static_cast<lg::Level>(lev));
}
}  // namespace

void lg::SetLevel(Level lev) {
  IsInitialized();
  current_level = lev;
}

void lg::SetHandler(Handler h) {
  current_handler = h;
  google::protobuf::SetLogHandler(&CheckLevel);
}

void lg::DefaultHandler(std::string const& message,
                        char const* source_file,
                        int source_line,
                        Level lev) {
  std::clog << source_file << ':' << source_line << ": " << LevelDescriptor(lev)
            << ": " << message << '\n';
  if (lev == Level::Fatal) {
    std::abort();
  }
}

std::string_view lg::LevelDescriptor(Level l) {
  switch (l) {
    case Level::Trace:
      return "trace";
    case Level::Debug:
      return "debug";
    case Level::Info:
      return "note";  // The next 3 are gcc- & clang-inspired
    case Level::Warn:
      return "warning";
    case Level::Error:
      return "error";
    case Level::Fatal:
      return " ### FATAL ERROR ### ";
    case Level::Off:
      return "off";
    default:
      return "Unknown log level used: possible corruption?";
  }
}

bool lg::IsInitialized() {
  if (current_handler) {
    return true;
  }
  SetHandler(&DefaultHandler);
  return false;
}
