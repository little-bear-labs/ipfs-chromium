#include "ipns_name.h"

#include "log_macros.h"

using Self = ipfs::ipld::IpnsName;

Self::IpnsName(std::string_view target_abs_path) {
  SlashDelimited target{target_abs_path};
  target_namespace_ = target.pop();
  target_root_ = target.pop();
  links_.emplace_back("", Link{target_root_, nullptr});
  target_path_.assign(target.to_string());
}

auto Self::resolve(ResolutionState& params) -> ResolveResult {
  auto& node = links_.at(0).second.node;
  if (!node) {
    node = params.GetBlock(target_root_);
  }
  if (!node) {
    return MoreDataNeeded(target_namespace_ + "/" + target_root_);
  }
  if (target_path_.empty()) {
    return node->resolve(params);
  }
  auto path = target_path_;
  path.append("/").append(params.PathToResolve().to_view());
  auto altered = params.WithPath(path);
  LOG(WARNING) << "Odd case: name points at /ns/root/MORE/PATH ("
               << target_namespace_ << '/' << target_root_ << '/'
               << target_path_ << "): " << params.MyPath()
               << " will be resolved as " << path;
  return node->resolve(params);
}
