#include "symlink.h"

#include "log_macros.h"

using Self = ipfs::ipld::Symlink;

Self::Symlink(std::string target) : target_{target} {}

Self::~Symlink() noexcept {}

auto Self::resolve(ResolutionState& params) -> ResolveResult {
  std::string result;
  if (!is_absolute()) {
    auto left_path = params.MyPath();
    left_path.pop_back();  // Because the final component refers to this
                           // symlink, which is getting replaced with target
    result.assign(left_path.to_view());
  }
  SlashDelimited target{target_};
  for (std::string_view component; (component = target.pop()).size();) {
    if (component == "..") {
      auto slash = result.rfind('/');
      DCHECK(slash < result.size());
      if (slash < result.size()) {
        result.resize(slash);
      } else {
        result.clear();
      }
    } else {
      result.append(1UL, '/').append(component);
    }
  }
  if (!params.IsFinalComponent()) {
    result.append("/").append(params.PathToResolve().to_string());
  }
  std::size_t i;
  while ((i = result.find("//")) != std::string::npos) {
    result.erase(i, 1);
  }
  if (result.empty()) {
    result.assign(1UL, '/');
  } else if (result.ends_with('/')) {
    result.resize(result.size() - 1);
  }
  VLOG(1) << "symlink: '" << params.MyPath() << "' -> '" << result << "'.";
  return PathChange{result};
}

bool Self::is_absolute() const {
  return target_.at(0) == '/';
}