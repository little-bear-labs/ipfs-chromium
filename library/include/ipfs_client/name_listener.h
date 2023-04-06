#ifndef IPFS_NAME_LISTENER_H_
#define IPFS_NAME_LISTENER_H_

#include <memory>

namespace ipfs {
class NameListener : public std::enable_shared_from_this<NameListener> {
 public:
  virtual void Complete() = 0;
};
}  // namespace ipfs

#endif  // IPFS_NAME_LISTENER_H_
