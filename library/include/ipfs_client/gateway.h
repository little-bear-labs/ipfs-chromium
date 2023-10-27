#ifndef CHROMIUM_IPFS_GATEWAY_H_
#define CHROMIUM_IPFS_GATEWAY_H_

#include "url_spec.h"

#include "vocab/flat_mapset.h"

#include <ctime>
#include <string>

namespace ipfs {

/*!
 * \brief A known http gateway serving up blocks & IPNS records
 */
class Gateway {
  std::string prefix_;
  flat_map<UrlSpec, std::time_t> failed_requests_;
  unsigned priority_;  /// This is not request priority. This is how eager we
                       /// are to use this particular gateway.
  flat_set<UrlSpec> tasks_;

 public:
  /*!
   * \brief Construct a new gateway
   * \param url_prefix - including at least full origin
   *    not including path parts like ipfs/ or ipns/ as those will be added
   *    It must end in a slash (/)
   * \param priority   - Performs 2 duties:
   *    * Relative to other gateways, how preferred it is for new requests.
   *    * How many concurrent requests before it's considered overloaded and
   *      completely ineligible.
   * \sa    [scoring](md_library_src_ipfs_client_scoring.html)
   */
  Gateway(std::string url_prefix, unsigned priority);
  Gateway(Gateway const&);
  ~Gateway();

  std::string const& url_prefix() const;  ///< Get this gateway's identity
  long load() const;  ///< How busy is it currently with outstanding requests

  /*!
   * \brief accept a new task
   * \param suffix - Appended to url_prefix() provides the URL of the request
   * \param need   - How important is this task
   * \return whether the task was accepted
   */
  bool accept(UrlSpec const& req, long need);

  /*!
   * \brief Indicate a task completed successfully
   * \param ts - The suffix of the task
   */
  void TaskSuccess(UrlSpec const& ts);

  /*!
   * \brief Indicate a task completed unsuccessfully
   * \param ts - The suffix of the task
   */
  void TaskFailed(UrlSpec const&);

  /*!
   * \brief Indicate a task was cancelled
   * \param ts - The suffix of the task
   */
  void TaskCancelled(UrlSpec const&);

  /*!
   * \brief Whether this gateway has previously failed to serve that task
   * \param suffix - The suffix of the task
   */
  bool PreviouslyFailed(UrlSpec const&) const;

  /*!
   * \return Whether this gateway should be generally preferred over another
   */
  bool operator<(Gateway const&) const;
};

}  // namespace ipfs

#endif  // CHROMIUM_IPFS_GATEWAY_H_
