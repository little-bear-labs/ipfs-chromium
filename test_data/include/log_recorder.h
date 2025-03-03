#ifndef LOG_RECORDER_H
#define LOG_RECORDER_H

#include <ipfs_client/logger.h>

#include <cassert>
#include <source_location>
#include <vector>

namespace {
class LogRecorder {
  std::string id_;
  ipfs::log::Level old_lev_;
 public:
  LogRecorder(std::source_location loc = {})
  : id_{loc.file_name()}
  , old_lev_{ipfs::log::GetLevel()}
  {
    id_.append(loc.function_name());
    ipfs::log::AddHook(id_, [this](auto m,auto f,auto l,auto n){Handle(m, f, l, n);});
    ipfs::log::SetLevel(ipfs::log::Level::Off);
  }
  ~LogRecorder() {
    ipfs::log::SetLevel(old_lev_);
    ipfs::log::Unhook(id_);
  }
  struct LogMessage {
    std::string message;
    std::string_view file;
    int line;
    ipfs::log::Level level;
  };
  std::vector<LogMessage> messages;
  std::string all;
  void Handle(std::string_view msg, std::string_view fil, int lin, ipfs::log::Level lev) {
    all.append(msg).push_back('\n');
    messages.push_back({std::string{msg}, fil, lin, lev});
  }
};
}

#endif // LOG_RECORDER_H
