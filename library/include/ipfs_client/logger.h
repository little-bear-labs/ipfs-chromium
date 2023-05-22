#ifndef IPFS_LOGGER_H_
#define IPFS_LOGGER_H_

#include <string>

namespace ipfs::log {

enum class Level {
  TRACE = -2,
  DEBUG = -1,
  INFO = 0,
  WARN = 1,
  ERROR = 2,
  FATAL = 3
};

void SetLevel(Level);

using Handler = void(*)(std::string const&,char const*,int,Level);
void SetHandler(Handler);

void DefaultHandler(std::string const& message, char const* source_file, int source_line, Level for_prefix);

std::string_view LevelDescriptor(Level);

bool IsInitialized();

}

#endif // LOGGER_H
