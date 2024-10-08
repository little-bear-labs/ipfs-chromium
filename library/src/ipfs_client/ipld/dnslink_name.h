#ifndef IPFS_IPLD_DNSLINK_NAME_H_
#define IPFS_IPLD_DNSLINK_NAME_H_

#include "ipfs_client/ipld/dag_node.h"

#include <chrono>

namespace ipfs::ipld {
class DnsLinkName : public DagNode {
  std::string target_namespace_;
  std::string target_root_;
  std::string target_path_;
  std::chrono::system_clock::time_point expiration_;

  ResolveResult resolve(ResolutionState& params) override;
  bool PreferOver(DagNode const& another) const override;
  DnsLinkName const* as_dnslink() const override { return this; }

 public:
  DnsLinkName(std::string_view target_abs_path);
  ~DnsLinkName() noexcept override;

  bool expired() const override;
};
}  // namespace ipfs::ipld

#endif  // IPFS_IPLD_DNSLINK_NAME_H_
