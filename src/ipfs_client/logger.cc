#include <ipfs_client/logger.h>

#undef LOGURU_WITH_STREAMS
#define LOGURU_WITH_STREAMS 1
#include <loguru.hpp>

#include <iostream>
#include <map>
#include <string>
#include <string_view>

namespace lg = ipfs::log;

namespace {
auto to_lev(int i) {
  i = std::max(-i, -2);
  i = std::min( i,  4);
  return static_cast<lg::Level>(i);
}
}

auto lg::GetLevel() -> Level {
  return to_lev(loguru::g_stderr_verbosity);
}
void lg::SetLevel(Level lev) {
  loguru::g_stderr_verbosity = -static_cast<int>(lev);
}

auto lg::LevelDescriptor(Level l) -> std::string_view {
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
namespace {
void adapt_hook(void* user_data, loguru::Message const& mes) {
  auto* h = reinterpret_cast<lg::Hook*>(user_data);
  auto lev = to_lev(mes.verbosity);
  (*h)(mes.message, mes.filename, mes.line, lev);
}
std::map<std::string,lg::Hook> hook_storage;
}
void lg::AddHook(std::string id, Hook hook) {
  auto& h = hook_storage[id];
  h = hook;
  loguru::add_callback(id.c_str(), adapt_hook, &h, loguru::NamedVerbosity::Verbosity_MAX);
}
void lg::Unhook(std::string id) {
  hook_storage.erase(id);
  loguru::remove_callback(id.c_str());
}
