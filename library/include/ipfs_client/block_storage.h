#ifndef IPFS_BLOCKS_H_
#define IPFS_BLOCKS_H_

#include "dag_block.h"
#include "vocab/flat_mapset.h"

#include <list>
#include <string>
#include <string_view>

namespace libp2p::multi {
struct ContentIdentifier;
}

namespace ipfs {
class DagListener;
class ContextApi;

class UnixFsPathResolver;

/*!
 * \brief Immediate access to recently-accessed blocks
 * \details Blocks are held in-memory, using pretty standard containers, as
 *    already-parsed ipfs::Block objects.
 */
class BlockStorage {
 public:
  BlockStorage();

  BlockStorage(BlockStorage const&) = delete;

  ~BlockStorage() noexcept;

  /*!
   * \brief Store a Block for later access.
   * \param cid_str - The string representation of cid
   * \param cid     - The Content IDentifier
   * \param headers - Associated HTTP headers
   * \param body    - The raw bytes of the block
   * \param block   - The block being stored
   * \return Whether this block is now stored in *this
   */
  bool Store(std::string cid_str,
             Cid const& cid,
             std::string headers,
             std::string const& body,
             Block&& block);

  /*!
   * \name Store (Convenience)
   * Convenience functions for
   * ipfs::BlockStorage::Store(std::string,Cid const&,std::string,std::string
   * const&,Block&&)
   */
  ///@{
  bool Store(std::string headers, std::string const& body, Block&& block);
  bool Store(std::string const& cid, std::string headers, std::string body);
  bool Store(std::string cid_str,
             Cid const& cid,
             std::string headers,
             std::string body);
  bool Store(Cid const& cid,
             std::string headers,
             std::string const& body,
             Block&&);
  ///@}

  /*!
   * \brief Get a block!
   * \details cid must match string-wise exactly: same multibase & all.
   *    For identity codecs, returns the data even if not stored.
   * \param cid - String representation of the CID for the block.
   * \return Non-owning pointer if found, nullptr
   * otherwise
   */
  Block const* Get(std::string const& cid);

  /*!
   * \brief Get HTTP headers associated with the block
   * \param cid - String representation of the CID for the block.
   * \return nullptr iff ! Get(cid) ;
   *    Empty string if the headers have never been set ;
   *    Otherwise, application-specific std::string (as-stored)
   */
  std::string const* GetHeaders(std::string const& cid);

  /*!
   * \brief Indicate that a particular path resolver is waiting on a CID to
   * become available
   */
  void AddListening(UnixFsPathResolver*);

  /*!
   * \brief Indicate that a particular path resolver is no longer waiting
   */
  void StopListening(UnixFsPathResolver*);

  /*!
   * \brief Normally called internally
   * \details Checks to see if any listening path resolver appears to be waiting
   * on a CID which is now available.
   */
  void CheckListening();

  /*!
   *  \brief Type for callbacks about new blocks
   *  \details The parameters to the hook are
   *    * CID string
   *    * HTTP headers
   *    * raw bytes of the block
   */
  using SerializedStorageHook =
      std::function<void(std::string, std::string, std::string)>;

  /*!
   * \brief Register a callback that will be called when any new block goes into
   * storage
   */
  void AddStorageHook(SerializedStorageHook);

 private:
  struct Record {
    Record();
    ~Record() noexcept;
    std::time_t last_access = 0L;
    std::string cid_str = {};
    Block block = {};
    std::string headers = {};
  };
  std::list<Record> records_ = std::list<Record>(0xFFUL);
  using Iter = decltype(records_)::iterator;
  flat_map<std::string, Record*> cid2record_;
  flat_set<UnixFsPathResolver*> listening_;
  bool checking_ = false;
  std::vector<SerializedStorageHook> hooks_;

  Record const* GetInternal(std::string const&);
  Record* FindFree(std::time_t);
  Record* Allocate();
  Record* StoreIdentity(std::string const&, Cid const&);
};
}  // namespace ipfs

#endif  // IPFS_BLOCKS_H_
