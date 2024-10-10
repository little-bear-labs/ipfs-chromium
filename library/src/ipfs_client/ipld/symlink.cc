#include "symlink.h"

#include <utility>
#include <string>
#include <string_view>
#include <cstddef>

#include "ipfs_client/ipld/resolution_state.h"
#include "ipfs_client/ipld/dag_node.h"
#include "log_macros.h"
#include "vocab/slash_delimited.h"

using Self = ipfs::ipld::Symlink;

Self::Symlink(std::string target) : target_{std::move(target)} {}

Self::~Symlink() noexcept = default;

auto Self::resolve(ResolutionState& params) -> ResolveResult {
  std::string result;
  if (!is_absolute()) {
    auto left_path = params.MyPath();
    left_path.pop_back();  // Because the final component refers to this
                           // symlink, which is getting replaced with target
    result.assign(left_path.to_view());
  }
  SlashDelimited target{target_};
  for (std::string_view component; static_cast<unsigned int>(!(component = target.pop()).empty()) != 0U;) {
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
  std::size_t pos;
  while ((pos = result.find("//")) != std::string::npos) {
    result.erase(pos, 1);
  }
  if (result.empty()) {
    result.assign(1UL, '/');
  } else if (result.ends_with('/')) {
    result.resize(result.size() - 1);
  }
  VLOG(1) << "symlink: '" << params.MyPath() << "' -> '" << result << "'.";
  return PathChange{result};
}

auto Self::is_absolute() const -> bool {
  return target_.at(0) == '/';
}
