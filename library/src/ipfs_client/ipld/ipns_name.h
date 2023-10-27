#ifndef IPFS_IPLD_IPNS_NAME_H_
#define IPFS_IPLD_IPNS_NAME_H_

#include "ipfs_client/ipld/dag_node.h"

namespace ipfs::ipld {
class IpnsName : public DagNode {
  std::string const target_path_;
  NodePtr target_;

  ResolveResult resolve(SlashDelimited path,
                        BlockLookup,
                        std::string& up_to_here) override;

 public:
  IpnsName(std::string target_abs_path);
  virtual ~IpnsName() noexcept {}
};
}  // namespace ipfs::ipld

#endif  // IPFS_IPLD_IPNS_NAME_H_
