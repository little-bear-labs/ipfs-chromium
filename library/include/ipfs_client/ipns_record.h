#ifndef IPFS_IPNS_RECORD_H_
#define IPFS_IPNS_RECORD_H_

#include <vocab/byte_view.h>

#include "components/ipfs/keys.pb.h"

#include <functional>
#include <optional>

namespace libp2p::peer {
class PeerId;
}

namespace ipfs {

/*!
 * \brief Parsed out data contained in the CBOR data of an IPNS record.
 */
struct IpnsCborEntry {
  std::string value;     ///< The "value" (target) the name points at
  std::string validity;  ///< Value to compare for validity (i.e. expiration)
  std::uint64_t validityType;  ///< Way to deterimine current validity
  std::uint64_t sequence;  ///< Distinguish other IPNS records for the same name
  std::uint64_t ttl;       ///< Recommended caching time
};

using CborDeserializer = IpnsCborEntry(ByteView);

using CryptoSignatureVerifier = bool(ipns::KeyType,
                                     ByteView,
                                     ByteView,
                                     ByteView);

std::optional<IpnsCborEntry> ValidateIpnsRecord(
    ByteView top_level_bytes,
    libp2p::peer::PeerId const& name,
    CryptoSignatureVerifier,
    CborDeserializer);

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
  ValidatedIpns(IpnsCborEntry const&);
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
