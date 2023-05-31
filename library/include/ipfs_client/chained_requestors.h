#ifndef CHAINED_REQUESTORS_H
#define CHAINED_REQUESTORS_H

#include "block_requestor.h"

#include <vector>

namespace ipfs {

/*!
 * \brief A BlockRequestor that chains together BlockRequestors
 * \details It iterates through the chain returning the first successful hit.
 */
class ChainedRequestors : public BlockRequestor {
 public:
  using Ptr = std::shared_ptr<BlockRequestor>;  ///< Owning pointer to the iface

  /*!
   * \brief Append a requestor to the end of the chain
   * \param p shared-ownership pointer to the requestor
   */
  void Add(Ptr);

  /*!
   * \brief Check if the chain appears valid
   * \return non-zero number of requestors in the chain
   */
  bool Valid() const;

  ChainedRequestors();
  ~ChainedRequestors() noexcept;

 private:
  std::vector<Ptr> chain_;

  /*!
   * \brief Implement BlockRequestor interface
   */
  void RequestByCid(std::string,
                    std::shared_ptr<DagListener>,
                    Priority) override;
};
}  // namespace ipfs

#endif  // CHAINED_REQUESTORS_H
