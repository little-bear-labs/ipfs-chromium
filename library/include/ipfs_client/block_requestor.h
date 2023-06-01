#ifndef BLOCK_REQUESTOR_H_
#define BLOCK_REQUESTOR_H_

#include <functional>
#include <memory>
#include <string>

namespace ipfs {

/*!
 * \brief The urgency of a gateway request
 * \details Determines how many gateways should be involved, and how burdened a
 *    gateway should be before not also taking this one on concurrently. Zero is
 *    a special value that indicates the block isn't actually required now, but
 *    rather might be required soonish (prefetch). There are some cases of
 *    special handling for that.
 */
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
   * \param cid      - MB-MH string representation of the Content IDentifier
   * \param dl       - Someone who may be interested
   * \param priority - Urgency of the request
   * \note The DagListener is mostly about lifetime extension, since it's
   *    waiting on something which is waiting on this
   */
  virtual void RequestByCid(std::string cid,
                            std::shared_ptr<DagListener> dl,
                            Priority priority) = 0;
};
}  // namespace ipfs

#endif  // BLOCK_REQUESTOR_H_
