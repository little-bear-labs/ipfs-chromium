#ifndef NETWORKREQUESTOR_H
#define NETWORKREQUESTOR_H

#include <ipfs_client/block_requestor.h>

namespace ipfs {

class InterRequestState;

class NetworkRequestor : public BlockRequestor {
  InterRequestState& state_;
 public:
  NetworkRequestor(InterRequestState& state);
  virtual ~NetworkRequestor() noexcept;

 private:
  void RequestByCid(std::string cid,
                    std::shared_ptr<DagListener>,
                    Priority) override;

};
}  // namespace ipfs

#endif  // NETWORKREQUESTOR_H
