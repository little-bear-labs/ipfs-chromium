#ifndef IPFS_DAG_LISTENER_H_
#define IPFS_DAG_LISTENER_H_

#include <memory>
#include <string>
#include <string_view>

namespace ipfs {
class DagListener : public std::enable_shared_from_this<DagListener> {
 public:
  // Sorry for the terrible name.
  //  You're receiving the next N bytes of a requested file, corresponding to a
  //  single block of its DAG
  virtual void ReceiveBlockBytes(std::string_view) = 0;
  // Again, sorry for this name.
  // The file you were recevining in ReceiveBlockBytes is done. You got all the
  // bytes.
  virtual void BlocksComplete(std::string mime_type) = 0;

  // The requested path DOES NOT EXIST, conclusively, regardless of gateway.
  virtual void FourOhFour(std::string_view, std::string_view) = 0;
};
}  // namespace ipfs

#endif
