#ifndef LOG_RECORDER_H
#define LOG_RECORDER_H

#include <ipfs_client/logger.h>

#include <cassert>
#include <vector>

namespace {
struct LogRecorder {
  static LogRecorder* me;
  LogRecorder() {
    assert(!me);
    me = this;
    ipfs::log::SetHandler(Handle);
  }
  ~LogRecorder() {
    ipfs::log::SetHandler(&ipfs::log::DefaultHandler);
    assert(me == this);
    me = nullptr;
  }
  struct LogMessage {
    std::string message;
    std::string_view file;
    int line;
    ipfs::log::Level level;
  };
  std::vector<LogMessage> messages;
  static void Handle(std::string const& msg, char const* fil, int lin, ipfs::log::Level lev) {
    assert(me);
    me->messages.push_back({msg, fil, lin, lev});
  }
};
LogRecorder* LogRecorder::me = nullptr;
}

#endif // LOG_RECORDER_H
