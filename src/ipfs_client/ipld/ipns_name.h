#ifndef IPFS_IPLD_IPNS_NAME_H_
#define IPFS_IPLD_IPNS_NAME_H_

#include "ipfs_client/ipld/dag_node.h"

#include <chrono>

namespace ipfs {
struct ValidatedIpns;
namespace ipld {
class IpnsName : public DagNode {
  std::string target_namespace_;
  std::string target_root_;
  std::string target_path_;
  std::chrono::system_clock::time_point expiration_;
  std::size_t serial_;

  ResolveResult resolve(ResolutionState& params) override;
  bool PreferOver(DagNode const& another) const override;
  IpnsName const* as_ipns() const override { return this; }

 public:
  /*! Construct from a validated entry
   */
  explicit IpnsName(ValidatedIpns const&);
  ~IpnsName() noexcept override;

  /*! @return whether this record should be considered expired
   *  @note Most useful for cached entries, but also can expire
   *    While sitting in the in-memory DAG, particularly by TTL
   */
  bool expired() const override;
};
}  // namespace ipld
}  // namespace ipfs

#endif  // IPFS_IPLD_IPNS_NAME_H_
