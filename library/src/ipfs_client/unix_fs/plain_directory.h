#ifndef IPFS_PLAIN_DIRECTORY_H_
#define IPFS_PLAIN_DIRECTORY_H_

#include "node_helper.h"

namespace ipfs {
class ContextApi;
class UnixFsPathResolver;
namespace unix_fs {
class PlainDirectory : public NodeHelper {
 public:
  PlainDirectory(std::string next_path_element);
  ~PlainDirectory() noexcept override;

 private:
  std::string const next_path_element_;

  bool Process(std::unique_ptr<NodeHelper>&,
               std::shared_ptr<DagListener>,
               std::function<void(std::string, Priority)>,
               std::string&) override;
};

}  // namespace unix_fs
}  // namespace ipfs

#endif  // IPFS_PLAIN_DIRECTORY_H_
