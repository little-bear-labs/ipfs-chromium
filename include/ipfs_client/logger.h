#ifndef IPFS_LOGGER_H_
#define IPFS_LOGGER_H_

#include <functional>
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

Level GetLevel();
void SetLevel(Level);

std::string_view LevelDescriptor(Level);

using Hook = std::function<void(std::string_view, std::string_view, int, Level)>;
void AddHook(std::string id, Hook);
void Unhook(std::string id);

}  // namespace ipfs::log

#endif  // LOGGER_H
