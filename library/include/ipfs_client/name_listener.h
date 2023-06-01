#ifndef IPFS_NAME_LISTENER_H_
#define IPFS_NAME_LISTENER_H_

#include <memory>

namespace ipfs {

/*!
 * \brief Implemented by classes that await IPNS and/or DNSLink resolution
 */
class NameListener : public std::enable_shared_from_this<NameListener> {
 public:

  /// Called when available in IpnsNames
  virtual void Complete() = 0;
};
}  // namespace ipfs

#endif  // IPFS_NAME_LISTENER_H_
