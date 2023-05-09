#ifndef IPFS_PLAIN_FILE_H_
#define IPFS_PLAIN_FILE_H_

#include "node_helper.h"

namespace ipfs {
class ContextApi;
class UnixFsPathResolver;
namespace unix_fs {
class PlainFile : public NodeHelper {
 public:
  PlainFile();
  ~PlainFile() noexcept override;

 private:
  bool Process(std::unique_ptr<NodeHelper>&,
               std::shared_ptr<DagListener>,
               std::function<void(std::string, Priority)>,
               std::string&) override;
};

}  // namespace unix_fs
}  // namespace ipfs

#endif  // IPFS_PLAIN_FILE_H_
