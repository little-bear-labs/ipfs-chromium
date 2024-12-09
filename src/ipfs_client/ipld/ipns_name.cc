#include "ipns_name.h"

#include <ipfs_client/ipns_record.h>
#include <chrono>
#include <algorithm>
#include <variant>

#include "ipfs_client/ipld/link.h"
#include "ipfs_client/ipld/resolution_state.h"
#include "ipfs_client/ipld/dag_node.h"
#include "log_macros.h"
#include "vocab/slash_delimited.h"

using Self = ipfs::ipld::IpnsName;
namespace ch = std::chrono;

Self::~IpnsName() noexcept = default;

Self::IpnsName(ValidatedIpns const& record)
    : expiration_{ch::system_clock::from_time_t(
          std::min(record.use_until, record.cache_until))},
      serial_{record.sequence} {
  SlashDelimited target{record.value};
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
    return node->Resolve(params);
  }
  auto path = target_path_;
  path.append("/").append(params.PathToResolve().to_view());
  auto altered = params.WithPath(path);
  LOG(WARNING) << "Odd case: name points at /ns/root/MORE/PATH ("
               << target_namespace_ << '/' << target_root_ << '/'
               << target_path_ << "): " << params.MyPath()
               << " will be resolved as " << path;
  auto lower = node->Resolve(params);
  if (auto* mdn = std::get_if<MoreDataNeeded>(&lower)) {
    if (expired()) {
      auto refresh_path = "/ipns/" + params.MyPath().pop_back();
      mdn->ipfs_abs_paths_.push_back(refresh_path);
    }
  }
  return lower;
}
auto Self::expired() const -> bool {
  return expiration_ < ch::system_clock::now();
}
auto Self::PreferOver(DagNode const& another) const -> bool {
  const auto *other = another.as_ipns();
  if (other == nullptr) {
    return true;
  }
  return serial_ > other->serial_;
}
