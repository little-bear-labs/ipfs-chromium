#ifndef IPNS_NAME_RESOLVER_H_
#define IPNS_NAME_RESOLVER_H_

#include <vocab/flat_mapset.h>

#include <string>

namespace ipfs {
class IpnsNames {
  flat_map<std::string, std::string> names_;

 public:
  IpnsNames();
  ~IpnsNames();

  std::string_view NameResolvedTo(std::string_view name) const;
  void AssignName(std::string const&, std::string);
  void NoSuchName(std::string const&);

  static constexpr std::string_view kNoSuchName{"NO_SUCH_NAME"};
};
}  // namespace ipfs

#endif  // IPNS_NAME_RESOLVER_H_
