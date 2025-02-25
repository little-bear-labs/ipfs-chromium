#include "gateway_state.h"

#include <ipfs_client/gw/gateway_request.h>
#include <string_view>

#include <utility>
#include <memory>
#include <ctime>
#include <cstddef>
#include <algorithm>
#include <limits>

#include "ipfs_client/gw/gateway_request_type.h"
#include "log_macros.h"

using Self = ipfs::gw::GatewayState;

Self::GatewayState(std::string_view prefix, std::shared_ptr<Client> api)
    : prefix_{prefix}, api_{std::move(api)} {
  DCHECK(!prefix_.empty());
  last_hist_update = std::time({});
  sent_counts.fill(0U);
}
// NOLINTBEGIN(readability-magic-numbers)
auto Self::score(GatewayRequest const& req, unsigned baseline) const -> long {
  auto result = static_cast<long>(baseline);
  result += 7L * cfg().GetTypeAffinity(prefix_, req.type);
  auto aff = affinity_success.find(req.affinity);
  if (aff != affinity_success.end()) {
    result += 8L * aff->second;
  }
  return result;
}
auto Self::over_rate(unsigned req_per_min) -> bool {
  return total_sent + current_bucket() > static_cast<std::size_t>(req_per_min) * MinutesTracked;
}
auto Self::over_rate() -> bool {
  return over_rate(cfg().GetGatewayRate(prefix_));
}
auto Self::bored() const -> bool {
  return total_sent == 0UL;
}
void Self::just_sent_one() {
  current_bucket()++;
  ++total_sent;
}
auto Self::current_bucket() -> unsigned int& {
  auto now = std::time({});
  while (last_hist_update < now) {
    ++last_hist_update;
    auto& count = sent_counts[last_hist_update % sent_counts.size()];
    total_sent -= count;
    count = 0;
  }
  return sent_counts[last_hist_update % sent_counts.size()];
}
void Self::hit(GatewayRequestType grt, GatewayRequest const& req) {
  auto& config = cfg();
  auto typaff = config.GetTypeAffinity(prefix_, grt);
  config.SetTypeAffinity(prefix_, grt, std::max(typaff + 9, 1));
  affinity_success[req.affinity] += 9;
  auto rpm = config.GetGatewayRate(prefix_);
  for (auto i = 14; i != 0; --i) {
    if (over_rate(++rpm / i)) {
      ++rpm;
    } else {
      break;
    }
  }
  config.SetGatewayRate(prefix_, rpm);
}
auto Self::miss(GatewayRequestType grt, GatewayRequest const& req) -> bool {
  auto& config = cfg();
  auto aff = config.GetTypeAffinity(prefix_, grt);
  if (aff > std::numeric_limits<decltype(aff)>::min()) {
    config.SetTypeAffinity(prefix_, grt, --aff);
  }
  auto rpm = config.GetGatewayRate(prefix_);
  if (rpm == 0U) {
    for (auto i = 0U;; ++i) {
      if (auto gw = config.GetGateway(i)) {
        auto& p = gw->prefix;
        if (p != prefix_) {
          config.SetGatewayRate(p, gw->rate + 1U);
        }
      } else {
        break;
      }
    }
  } else if (!over_rate(rpm)) {
    config.SetGatewayRate(prefix_, rpm - 1);
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
  if ((--rpm != 0U) && !over_rate(rpm / 2)) {
    --rpm;
  }
  c.SetGatewayRate(prefix_, rpm);
}
// NOLINTEND(readability-magic-numbers)
auto Self::cfg() -> ctx::GatewayConfig& {
  DCHECK(api_);
  return api_->gw_cfg();
}
auto Self::cfg() const -> ctx::GatewayConfig const& {
  DCHECK(api_);
  return api_->gw_cfg();
}
