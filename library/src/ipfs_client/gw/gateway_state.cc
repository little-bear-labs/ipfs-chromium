#include "gateway_state.h"

#include <ipfs_client/gw/gateway_request.h>

#include "log_macros.h"

using Self = ipfs::gw::GatewayState;

Self::GatewayState(std::string_view prefix, std::shared_ptr<Client> api)
    : prefix_{prefix}, api_{api} {
  DCHECK(!prefix_.empty());
  last_hist_update = std::time({});
  sent_counts.fill(0U);
}
long Self::score(GatewayRequest const& req, unsigned baseline) const {
  auto result = static_cast<long>(baseline);
  result += 4L * cfg().GetTypeAffinity(prefix_, req.type);
  auto i = affinity_success.find(req.affinity);
  if (i != affinity_success.end()) {
    result += 3L * i->second;
  }
  return result;
}
bool Self::over_rate(unsigned req_per_min) {
  return total_sent + current_bucket() > req_per_min * MinutesTracked;
}
bool Self::over_rate() {
  return over_rate(cfg().GetGatewayRate(prefix_));
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
void Self::hit(GatewayRequestType grt, GatewayRequest const& req) {
  auto& c = cfg();
  auto typaff = c.GetTypeAffinity(prefix_, grt);
  c.SetTypeAffinity(prefix_, grt, std::max(typaff + 9, typaff / 9));
  affinity_success[req.affinity] += 9;
  auto rpm = c.GetGatewayRate(prefix_);
  if (!over_rate(rpm / 4)) {
    return;
  }
  if (over_rate(rpm / 3)) {
    ++rpm;
  }
  if (over_rate(rpm / 2)) {
    ++rpm;
  }
  if (over_rate(rpm++)) {
    ++rpm;
  }
  c.SetGatewayRate(prefix_, rpm);
}
bool Self::miss(GatewayRequestType grt, GatewayRequest const& req) {
  auto& c = cfg();
  auto aff = c.GetTypeAffinity(prefix_, grt);
  if (aff > std::numeric_limits<decltype(aff)>::min()) {
    c.SetTypeAffinity(prefix_, grt, --aff);
  }
  auto rpm = c.GetGatewayRate(prefix_);
  if (!rpm) {
    for (auto i = 0U;; ++i) {
      if (auto gw = c.GetGateway(i)) {
        auto& p = gw->prefix;
        if (p != prefix_) {
          c.SetGatewayRate(p, gw->rate + 1U);
        }
      } else {
        break;
      }
    }
  } else if (!over_rate(rpm)) {
    c.SetGatewayRate(prefix_, rpm - 1);
  }
  return affinity_success[req.affinity]-- >= 0;
}
void Self::timed_out() {
  slowness += 10;
  auto& c = cfg();
  auto rpm = c.GetGatewayRate(prefix_);
  if (over_rate(rpm)) {
    return;
  }
  if (--rpm && !over_rate(rpm / 2)) {
    --rpm;
  }
  c.SetGatewayRate(prefix_, rpm);
}
auto Self::cfg() -> ctx::GatewayConfig& {
  DCHECK(api_);
  return api_->gw_cfg();
}
auto Self::cfg() const -> ctx::GatewayConfig const& {
  DCHECK(api_);
  return api_->gw_cfg();
}
