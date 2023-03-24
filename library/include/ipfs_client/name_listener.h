#ifndef IPFS_NAME_LISTENER_H_
#define IPFS_NAME_LISTENER_H_

namespace ipfs {
class NameListener {
 public:
  virtual void Complete() = 0;
};
}  // namespace ipfs

#endif  // IPFS_NAME_LISTENER_H_
