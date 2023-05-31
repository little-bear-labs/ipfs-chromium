#ifndef IPFS_DAG_LISTENER_H_
#define IPFS_DAG_LISTENER_H_

#include <memory>
#include <string>
#include <string_view>

namespace ipfs {

/*!
 * \brief Implemented by things waiting on a particular path through a DAG
 */
class DagListener : public std::enable_shared_from_this<DagListener> {
 public:
  /*!
   * \brief   You're receiving the next N bytes of a requested file
   * \details This is called once per block, which may be the entire file.
   * \param   bytes - the bytes you are receiving
   */
  virtual void ReceiveBlockBytes(std::string_view bytes) = 0;

  /*!
   *  \brief The file you were receiving in ReceiveBlockBytes is done.
   *  \param mime_type - a guess at what the mime type of the file may be
   */
  virtual void BlocksComplete(std::string mime_type) = 0;

  /*!
   * \brief   There was a general failure getting the blocks.
   * \details Though used internally, when viewed externally this usually means
   *    you're requesting a CID which was simply not found on your gateways.
   * \param   cid  - String representation of the root of the DAG
   * \param   path - The path that could not be resolved.
   * \todo    Unfortunately right now path is always the full requested path.
   *    If an intermediary directory failed, it should probably just be the
   *    path up to that point. And may in the future be so.
   */
  virtual void NotHere(std::string_view cid, std::string_view path) = 0;

  /*!
   * \brief   The requested path DOES NOT EXIST, conclusively, regardless of gateway.
   * \details Cannot occur without a path, as it indicates a directory (sharded or
   *    otherwise) was successfully retrieved but lacked the link necessary to
   *    descend farther down the path.
   * \param   cid  - String representation of the root of the DAG
   * \param   path - The path that could not be resolved.
   * \sa      NotHere - the parameters carry the same meaning.
   */
  virtual void DoesNotExist(std::string_view cid, std::string_view path) = 0;
};
}  // namespace ipfs

#endif
