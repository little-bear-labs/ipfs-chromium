#include "dnslink_name.h"

#include "log_macros.h"

using Self = ipfs::ipld::DnsLinkName;
namespace ch = std::chrono;

Self::DnsLinkName(std::string_view target_abs_path)
    : expiration_(ch::system_clock::now() + ch::minutes(5)) {
  SlashDelimited target{target_abs_path};
  target_namespace_ = target.pop();
  target_root_ = target.pop();
  links_.emplace_back("", Link{target_root_, nullptr});
  target_path_.assign(target.to_string());
}
Self::~DnsLinkName() noexcept = default;

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
bool Self::expired() const {
  return expiration_ < ch::system_clock::now();
}
bool Self::PreferOver(DagNode const& another) const {
  auto* other = another.as_dnslink();
  if (!other) {
    return true;
  }
  return expiration_ > other->expiration_ + ch::seconds(1);
}
