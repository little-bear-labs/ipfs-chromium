#include "symlink.h"

#include "log_macros.h"

using Self = ipfs::ipld::Symlink;

Self::Symlink(std::string target) : target_{target} {}

Self::~Symlink() {}

auto Self::resolve(SlashDelimited path, BlockLookup, std::string& to_here)
    -> ResolveResult {
  std::string result;
  if (is_absolute()) {
    result.assign(SlashDelimited{to_here}.pop_n(2)).append("/").append(target_);
    LOG(INFO) << "Absolute (relative-to-dag-root) symlink '" << target_
              << "' leads to '" << result << "' ... ";
  } else {
    auto c = to_here.find_last_not_of('/');
    c = to_here.find_last_of('/', c);
    DCHECK(c != to_here.size()) << to_here;
      result.assign(to_here, 0, c + 1).append(target_);
      LOG(INFO) << "Relative symlink '" << target_ << "' leads to '" << result
                << "' ... ";
  }
  if (path) {
    result.append("/").append(path.pop_all());
  }
  std::size_t i;
  while ((i = result.find("//")) != std::string::npos) {
    result.erase(i, 1);
  }
  DCHECK_GT(result.size(), 0U);
  if (result.back() == '/') {
    result.resize(result.size() - 1);
  }
  LOG(INFO) << "symlink: '" << to_here << "' -> '" << result << "'.";
  return PathChange{result};
}

bool Self::is_absolute() const {
  return target_.at(0) == '/';
}