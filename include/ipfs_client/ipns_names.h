#ifndef IPNS_NAME_RESOLVER_H_
#define IPNS_NAME_RESOLVER_H_

#include <ipfs_client/ipns_record.h>
#include <vocab/flat_mapset.h>

#include <string>

namespace ipfs {

/*!
 * \brief Fast synchronous access to IPNS & DNSLink name resolution
 */
class IpnsNames {
  flat_map<std::string, ValidatedIpns> names_;

 public:
  IpnsNames();
  ~IpnsNames();

  /*!
   * \brief Get the already-known "value"/target of a given name
   * \param name - either a mb-mf IPNS (key) name, or a host with DNSLink
   * \return
   *    * if resolution is incomplete: ""
   *    * if it is known not to resolve: kNoSuchName
   *    * otherwise an IPFS path witout leading /, e.g.:
   *      - ipfs/bafybeicfqz46dj67nkhxaylqd5sknnidsr4oaw4hhsjrgdmcwt73sow2d4/
   *      - ipns/k51qzi5uqu5dlvj2baxnqndepeb86cbk3ng7n3i46uzyxzyqj2xjonzllnv0v8
   */
  std::string_view NameResolvedTo(std::string_view name) const;

  /*!
   * \brief Store an IPNS record that already validated for this name
   * \param name - The name that resolves with this
   * \param rec  - The record modulo validation bits
   */
  void AssignName(std::string const& name, ValidatedIpns rec);

  /*!
   * \brief Assign a target path to a DNSLink host
   * \param host   - The original host NOT including a "_dnslink." prefix
   * \param target - an IPFS path witout leading /
   */
  void AssignDnsLink(std::string const& host, std::string_view target);

  /*!
   * \brief   Store the definitive absence of a resolution
   * \details This is useful because code will check resolution here before
   *    trying to resolve it fresh again, and you can stop that if you know
   *    it will never work.
   */
  void NoSuchName(std::string const& name);

  /*!
   * \brief Fetch the all the stored IPNS record data
   * \param name - the IPNS name it was stored with
   * \return nullptr if missing, otherwise non-owning pointer to record
   */
  ValidatedIpns const* Entry(std::string const& name);

  /*!
   * \brief A special value constant
   */
  static constexpr std::string_view kNoSuchName{"NO_SUCH_NAME"};
};
}  // namespace ipfs

#endif  // IPNS_NAME_RESOLVER_H_
