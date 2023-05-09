#ifndef CHAINED_REQUESTORS_H
#define CHAINED_REQUESTORS_H

#include "block_requestor.h"

#include <vector>

namespace ipfs {
class ChainedRequestors : public BlockRequestor {
 public:
  using Ptr = std::shared_ptr<BlockRequestor>;
  void Add(Ptr);

  bool Valid() const;

  ChainedRequestors();
  ~ChainedRequestors() noexcept;

 private:
  std::vector<Ptr> chain_;

  void RequestByCid(std::string,
                    std::shared_ptr<DagListener>,
                    Priority) override;
};
}  // namespace ipfs

#endif  // CHAINED_REQUESTORS_H
