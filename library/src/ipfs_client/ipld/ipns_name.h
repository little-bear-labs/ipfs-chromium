#ifndef IPFS_IPLD_IPNS_NAME_H_
#define IPFS_IPLD_IPNS_NAME_H_

#include "ipfs_client/ipld/dag_node.h"

namespace ipfs::ipld {
class IpnsName : public DagNode {
  std::string target_namespace_;
  std::string target_root_;
  std::string target_path_;

  ResolveResult resolve(ResolutionState& params) override;

 public:
  IpnsName(std::string_view target_abs_path);
  virtual ~IpnsName() noexcept {}
};
}  // namespace ipfs::ipld

#endif  // IPFS_IPLD_IPNS_NAME_H_
