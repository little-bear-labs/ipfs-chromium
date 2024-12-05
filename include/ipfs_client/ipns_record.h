#ifndef IPFS_IPNS_RECORD_H_
#define IPFS_IPNS_RECORD_H_

#include <ipfs_client/ipns_cbor_entry.h>

#include <vocab/byte_view.h>

#if __has_include(<third_party/ipfs_client/keys.pb.h>)
#include <third_party/ipfs_client/keys.pb.h>
#else
#include "ipfs_client/keys.pb.h"
#endif

#include <functional>
#include <optional>

namespace libp2p::peer {
class PeerId;
}
namespace libp2p::multi {
struct ContentIdentifier;
}

namespace ipfs {

class Cid;
class Client;

constexpr static std::size_t MAX_IPNS_PB_SERIALIZED_SIZE = 10 * 1024;

std::optional<IpnsCborEntry> ValidateIpnsRecord(ByteView top_level_bytes,
                                                Cid const& name,
                                                Client&);

/*!
 * \brief Data from IPNS record modulo the verification parts
 */
struct ValidatedIpns {
  std::string value;  ///< The path the record claims the IPNS name points to
  std::time_t use_until;    ///< An expiration timestamp
  std::time_t cache_until;  ///< Inspired by TTL

  /*!
   * \brief   The version of the record
   * \details Higher sequence numbers obsolete lower ones
   */
  std::uint64_t sequence;
  std::int64_t resolution_ms;  ///< How long it took to fetch the record

  /*!
   * \brief When the record was fetched
   */
  std::time_t fetch_time = std::time(nullptr);
  std::string gateway_source;  ///< Who gave us this record?

  ValidatedIpns();  ///< Create an invalid default object
  /*! @param entry IPNS entry known to be valid
   */
  explicit ValidatedIpns(IpnsCborEntry const& entry);
  ValidatedIpns(ValidatedIpns&&);
  ValidatedIpns(ValidatedIpns const&);
  ValidatedIpns& operator=(ValidatedIpns const&);

  std::string Serialize() const;  ///< Turn into a well-defined list of bytes

  /*!
   * \brief  Create a ValidatedIpns from untyped bytes
   * \param  bytes - Output from a former call to Serialize()
   * \note   Is used by disk cache
   * \return Recreation of the old object
   */
  static ValidatedIpns Deserialize(std::string bytes);
};

}  // namespace ipfs

#endif  // IPFS_IPNS_RECORD_H_
