#ifndef BLOCK_REQUESTOR_H_
#define BLOCK_REQUESTOR_H_

#include <functional>
#include <memory>
#include <string>

namespace ipfs {

using Priority = std::uint_least16_t;

class DagListener;

/*!
 * \brief Interface for classes that can asynchronously fetch a block for a CID
 * \details This is one of the interfaces using code is meant to implement.
 *    Common usages:
 *      * A class that requests blocks from gateways
 *      * A cache that must act asynchronously (perhaps on-disk)
 *      * ChainedRequestors : a chain-of-responsibility combining multiple
 */
class BlockRequestor {
 public:
  /**
   * \brief Request a single block from gateway(s).
   * \note The DagListener is mostly about lifetime extension, since it's
   *    waiting on something which is waiting on this
   */
  virtual void RequestByCid(std::string cid,
                            std::shared_ptr<DagListener>,
                            Priority priority) = 0;
};
}  // namespace ipfs

#endif  // BLOCK_REQUESTOR_H_
