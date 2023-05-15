#ifndef IPNS_NAME_RESOLVER_H_
#define IPNS_NAME_RESOLVER_H_

#include <ipfs_client/ipns_record.h>
#include <vocab/flat_mapset.h>

#include <string>

namespace ipfs {
class IpnsNames {
  flat_map<std::string, ValidatedIpns> names_;

 public:
  IpnsNames();
  ~IpnsNames();

  std::string_view NameResolvedTo(std::string_view name) const;
  void AssignName(std::string const&, ValidatedIpns);
  void AssignDnsLink(std::string const&, std::string_view);
  void NoSuchName(std::string const&);
  ValidatedIpns const* Entry(std::string const& name);

  static constexpr std::string_view kNoSuchName{"NO_SUCH_NAME"};
};
}  // namespace ipfs

#endif  // IPNS_NAME_RESOLVER_H_
