#include "ipns_name.h"

#include "log_macros.h"

using Self = ipfs::ipld::IpnsName;

Self::IpnsName(std::string_view target_abs_path)
    : target_path_{target_abs_path} {}

auto Self::resolve(ipfs::SlashDelimited path,
                   ipfs::ipld::DagNode::BlockLookup blu,
                   std::string& up_to_here) -> ResolveResult {
  if (!target_) {
    SlashDelimited t{target_path_};
    t.pop();  // Discard namespace, though realistically it's going to be ipfs
              // basically all the time
    target_ = blu(std::string{t.pop()});
  }
  if (target_) {
    return target_->resolve(path, blu, up_to_here);
  }
  return MoreDataNeeded{std::vector{target_path_}};
}