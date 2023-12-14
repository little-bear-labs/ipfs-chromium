#include "ipns_name.h"

#include "log_macros.h"

using Self = ipfs::ipld::IpnsName;

Self::IpnsName(std::string_view target_abs_path)
    : target_path_{target_abs_path} {}

auto Self::resolve(ResolutionState& params) -> ResolveResult {
  // Can't use PathChange, as the target is truly absolute (rootless)
  SlashDelimited t{target_path_};
  t.pop();  // Discard namespace, though realistically it's going to be ipfs
            // basically all the time
  auto name = t.pop();
  if (t) {
    LOG(WARNING) << "Odd case: name points at /ns/root/MORE/PATH ("
                 << target_path_ << "): " << params.MyPath();
    auto path = t.to_string() + "/" + params.PathToResolve().to_string();
    auto altered = params.WithPath(path);
    return CallChild(altered, "", name);
  } else {
    return CallChild(params, "", name);
  }
}
