#ifndef IPFS_SYMLINK_H_
#define IPFS_SYMLINK_H_

#include <ipfs_client/ipld/dag_node.h>

namespace ipfs::ipld {
class Symlink : public DagNode {
  enum class Style {
    Relative,
    Absolute,
    FromRoot,
  };
  Style const style_;
  std::string const target_;

  Style from_target(std::string const&);
  ResolveResult resolve(SlashDelimited path,
                        BlockLookup,
                        std::string& up_to_here) override;

 public:
  Symlink(std::string target);
  ~Symlink() noexcept override;
};
}  // namespace ipfs::ipld

#endif  // IPFS_SYMLINK_H_
