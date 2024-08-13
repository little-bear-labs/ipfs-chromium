#ifndef IPFS_LOGGER_H_
#define IPFS_LOGGER_H_

#include <string>

namespace ipfs::log {

enum class Level {
  Trace = -2,
  Debug = -1,
  Info = 0,
  Warn = 1,
  Error = 2,
  Fatal = 3,
  Off
};

void SetLevel(Level);

using Handler = void (*)(std::string const&, char const*, int, Level);
void SetHandler(Handler);

void DefaultHandler(std::string const& message,
                    char const* source_file,
                    int source_line,
                    Level for_prefix);

std::string_view LevelDescriptor(Level);

bool IsInitialized();

}  // namespace ipfs::log

#endif  // LOGGER_H
