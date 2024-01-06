#include "gateway_state.h"

#include <ipfs_client/gw/gateway_request.h>

#include "log_macros.h"

using Self = ipfs::gw::GatewayState;

Self::GatewayState() {
  request_type_success.fill(0L);
  last_hist_update = std::time({});
  sent_counts.fill(0U);
}
long Self::score(GatewayRequest const& req, unsigned baseline) const {
  auto result = static_cast<long>(baseline);
  result += 2 * request_type_success.at(static_cast<std::size_t>(req.type));
  auto i = affinity_success.find(req.affinity);
  if (i != affinity_success.end()) {
    result += 3 * i->second;
  }
  return result;
}
bool Self::over_rate(unsigned req_per_min) {
  return total_sent + current_bucket() > req_per_min * MinutesTracked;
}
bool Self::bored() const {
  return total_sent == 0UL;
}
void Self::just_sent_one() {
  current_bucket()++;
  ++total_sent;
}
unsigned int& Self::current_bucket() {
  auto now = std::time({});
  while (last_hist_update < now) {
    ++last_hist_update;
    auto& c = sent_counts[last_hist_update % sent_counts.size()];
    total_sent -= c;
    c = 0;
  }
  return sent_counts[last_hist_update % sent_counts.size()];
}
void Self::hit(GatewayRequest const& req) {
  request_type_success.at(static_cast<std::size_t>(req.type))++;
  affinity_success[req.affinity]++;
}
bool Self::miss(GatewayRequest const& req) {
  request_type_success.at(static_cast<std::size_t>(req.type))--;
  slowness++;
  return affinity_success[req.affinity]-- >= 0;
}
void Self::timed_out() {
  slowness += 10;
}