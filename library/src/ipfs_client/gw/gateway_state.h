#ifndef IPFS_GATEWAY_STATE_H_
#define IPFS_GATEWAY_STATE_H_

#include <vocab/flat_mapset.h>

#include <ctime>

#include <array>
#include <string>

namespace ipfs::gw {
class GatewayRequest;
class GatewayState {
  flat_map<std::string, long> affinity_success;
  std::array<long, 7> request_type_success;
  static constexpr short MinutesTracked = 4;
  std::array<unsigned, MinutesTracked * 60UL> sent_counts;
  std::size_t total_sent = 0UL;
  std::time_t last_hist_update;
  unsigned& current_bucket();
  long slowness = 0;

 public:
  GatewayState();
  long score(GatewayRequest const&, unsigned) const;
  bool over_rate(unsigned req_per_min);
  bool bored() const;

  void just_sent_one();
  void hit(GatewayRequest const&);
  bool miss(GatewayRequest const&);
  void timed_out();
  long extra_ms() { return slowness; }
};
}  // namespace ipfs::gw

#endif  // IPFS_GATEWAY_STATE_H_
