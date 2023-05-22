#ifndef CHROMIUM_IPFS_GATEWAY_H_
#define CHROMIUM_IPFS_GATEWAY_H_

#include "vocab/flat_mapset.h"

#include <string>

namespace ipfs {

class Gateway {
  std::string prefix_;
  flat_set<std::string> failed_requests_;
  unsigned priority_;  /// This is not request priority. This is how eager we
                       /// are to use this particular gateway.
  flat_set<std::string> tasks_;

 public:
  Gateway(std::string url_prefix, unsigned priority);
  Gateway(Gateway const&);
  ~Gateway();

  std::string const& url_prefix() const;
  long load() const;

  bool accept(std::string const& suffix, long need);
  void TaskSuccess(std::string const&);
  void TaskFailed(std::string const&);
  void TaskCancelled(std::string const&);
  bool PreviouslyFailed(std::string const& suffix) const;

  bool operator<(Gateway const&) const;
};

}  // namespace ipfs

#endif  // CHROMIUM_IPFS_GATEWAY_H_
