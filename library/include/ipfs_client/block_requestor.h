#ifndef BLOCK_REQUESTOR_H_
#define BLOCK_REQUESTOR_H_

#include <functional>
#include <memory>
#include <string>

namespace ipfs {

using Priority = std::uint_least16_t;

class DagListener;

class BlockRequestor {
 public:
  // Request a single block from gateway(s).
  // The DagListener is mostly about lifetime extension, since it's waiting on
  // something which is waiting on this
  virtual void RequestByCid(std::string cid,
                            std::shared_ptr<DagListener>,
                            Priority priority) = 0;
};
}  // namespace ipfs

#endif  // BLOCK_REQUESTOR_H_
