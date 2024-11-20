#ifndef IPFS_GATEWAY_STATE_H_
#define IPFS_GATEWAY_STATE_H_

#include <ipfs_client/gw/gateway_request_type.h>

#include <ctime>

#include <array>
#include <memory>
#include <string>
#include <unordered_map>

namespace ipfs {
class Client;

namespace ctx {
class GatewayConfig;
}
}  // namespace ipfs

namespace ipfs::gw {
class GatewayRequest;
/*! Information about an IPFS HTTP gateway and how often it should be used for what
 */
class GatewayState {
  std::string prefix_;
  std::shared_ptr<Client> api_;
  std::unordered_map<std::string, long> affinity_success;
  static constexpr short MinutesTracked = 4;
  std::array<unsigned, MinutesTracked * 60UL> sent_counts;
  std::size_t total_sent = 0UL;
  std::time_t last_hist_update;
  unsigned& current_bucket();
  long slowness = 0;
  bool over_rate(unsigned req_per_min);

  ctx::GatewayConfig& cfg();
  ctx::GatewayConfig const& cfg() const;

 public:
  /*! @param prefix The URL prefix that goes in front of HTTP requests to the gateway
   *    e.g. https://ipfs.io/
   *  @param api Access to the functionality of this client's context
   */
  GatewayState(std::string_view prefix, std::shared_ptr<Client> api);
  long score(GatewayRequest const&, unsigned) const;
  bool bored() const;
  bool over_rate();

  void just_sent_one();
  void hit(GatewayRequestType, GatewayRequest const&);
  bool miss(GatewayRequestType, GatewayRequest const&);
  void timed_out();
  long extra_ms() { return slowness; }
};
}  // namespace ipfs::gw

#endif  // IPFS_GATEWAY_STATE_H_
