#ifndef IPFS_MULTI_NODE_FILE_H_
#define IPFS_MULTI_NODE_FILE_H_

#include "node_helper.h"

#include <optional>
#include <vector>

namespace ipfs {
namespace unix_fs {
class MultiNodeFile : public NodeHelper {
 public:
  MultiNodeFile();
  MultiNodeFile(MultiNodeFile const&);
  ~MultiNodeFile() noexcept override;

 private:
  std::vector<std::pair<std::string, std::optional<MultiNodeFile>>> children_;
  std::size_t written_until_ = 0UL;

  bool Process(std::unique_ptr<NodeHelper>&,
               std::shared_ptr<DagListener>,
               std::function<void(std::string, Priority)>,
               std::string&) override;

  void Fetch(std::function<void(std::string, Priority)>);
  bool Write(std::shared_ptr<DagListener>);
  std::string FirstChunk() const;
};

}  // namespace unix_fs
}  // namespace ipfs

#endif  // IPFS_MULTI_NODE_FILE_H_
