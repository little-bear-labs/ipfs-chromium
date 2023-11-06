#ifndef IPFS_IPNS_CBOR_ENTRY_H_
#define IPFS_IPNS_CBOR_ENTRY_H_

#include <cstdint>
#include <string>

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

}  // namespace ipfs

#endif  // IPFS_IPNS_CBOR_ENTRY_H_
