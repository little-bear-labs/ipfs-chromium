#include "multi_gateway_requestor.h"

#include <ipfs_client/gw/gateway_request_type.h>
#include <ipfs_client/ipfs_request.h>

#include <ipfs_client/gw/requestor.h>
#include <ipfs_client/gw/gateway_state.h>
#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/http_request_description.h>
#include <ipfs_client/ipld/block_source.h>

#include "log_macros.h"

#include <cctype>
#include <climits>
#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <chrono>
#include <functional>
#include <iterator>
#include <string>
#include <string_view>
#include <tuple>

using Self = ipfs::gw::MultiGatewayRequestor;

namespace ch = std::chrono;

auto Self::name() const -> std::string_view {
  return "multi-gateway requestor";
}
auto Self::handle(RequestPtr r) -> HandleOutcome {
  if (!r || !api_) {
    return HandleOutcome::DONE;
  }
  if (!r->is_http()) {
    LOG(INFO) << r->debug_string() << " is not an HTTP request.";
    return HandleOutcome::NOT_HANDLED;
  }
  Next();
  Process(r);
  return HandleOutcome::PENDING;
}
void Self::Next() {
  if (!q.empty()) {
    auto popped = q.front();
    q.pop_front();
    Process(popped);
  }
}
auto Self::Process(RequestPtr const& req) -> bool {
  if (!req->is_http()) {
    return false;
  }
  auto state_iter = state_.begin();
  auto config_idx = 0U;
  using Candidate = std::tuple<long, std::string, GatewayState*>;
  std::vector<Candidate> candidates;
  auto bored = 0U;
  Candidate over_rate = {0, {}, nullptr};
  auto& gws = api_->gw_cfg();
  while (auto gw = gws.GetGateway(config_idx++)) {
    if (state_iter == state_.end() || state_iter->first > gw->prefix) {
      // One can insert like this because state_ is std::map w/ stable iterators
      state_iter =
          state_.insert({gw->prefix, GatewayState{gw->prefix, api_}}).first;
      candidates.emplace_back(LONG_MAX, gw->prefix, &(state_iter->second));
    } else if (state_iter->first < gw->prefix) {
      LOG(INFO) << "Gateway has disappeared: " << state_iter->first
                << " (would've come before " << gw->prefix << ')';
      // TODO remove permanently, but without crashing any existing request that
      // will respond to this gateway
      //      auto to_rm = state_iter;
      std::advance(state_iter, 1);
      //      state_.erase(to_rm);
      continue;
    } else if (req->failures.contains(gw->prefix)) {
    } else if (state_iter->second.over_rate()) {
      auto score = state_iter->second.score(*req, gw->rate);
      if (std::get<0>(over_rate) <= score) {
        over_rate = std::make_tuple(score, gw->prefix, &(state_iter->second));
      }
    } else {
      candidates.emplace_back(state_iter->second.score(*req, gw->rate),
                            gw->prefix, &(state_iter->second));
      if (state_iter->second.bored()) {
        ++bored;
      }
    }
    std::advance(state_iter, 1);
  }
  if (std::get<2>(over_rate) != nullptr) {
    candidates.push_back(over_rate);
  }
  if (candidates.empty() && config_idx <= req->failures.size()) {
    LOG(ERROR) << "Run out of gateways to send this to:" << req->debug_string();
    forward(req);
    return false;
  }
  auto min_plel = req->type == GatewayRequestType::Block ? 3UL : 1UL;
  auto to_send = std::max(bored / 2UL, min_plel);
  std::sort(candidates.begin(), candidates.end(), std::greater{});
  static std::size_t extra = 0UL;
  auto send = [&](auto i){
      auto [score, prefix, state] = candidates[i];
      DoSend(req, prefix, *state);
  };
  for (auto idx = 0UL; idx < candidates.size(); ++idx) {
    if (idx > to_send) {
      if (idx + extra < candidates.size()) {
        send(idx + extra++);
      } else {
        extra = 0;
      }
      return true;
    } else {
      send(idx);
    }
  }
  q.push_back(req);
  return false;
}
void Self::DoSend(RequestPtr req, std::string const& gw, GatewayState& state) {
  DCHECK(!gw.empty());
  auto desc = req->describe_http(gw);
  if (!desc.has_value()) {
    LOG(ERROR) << "A request that has no HTTP description got pretty far "
                  "toward doing an HTTP request: "
               << req->debug_string();
    return;
  }
  if (state.extra_ms() != 0) {
    desc->timeout_seconds += state.extra_ms() / 1000L + 1L;
  }
  auto start = ch::system_clock::now();
  auto timeout_threshold =
      ch::seconds(desc->timeout_seconds != 0 ? desc->timeout_seconds : 300) -
      ch::milliseconds(1);
  auto hold_alive = shared_from_this();
  auto cb = [this, hold_alive, req, gw, timeout_threshold, desc, start](
                std::int16_t s, std::string_view b, auto h) {
    auto timed_out = ch::system_clock::now() - start >= timeout_threshold;
    HandleResponse(*desc, req, gw, s, b, h, timed_out, start);
  };
  state.just_sent_one();
  auto cancel = api_->http().SendHttpRequest(*desc, cb);
  if (req->dependent) {
    req->dependent->to_cleanup(cancel);
  }
}
void Self::HandleResponse(HttpRequestDescription const& desc,
                          RequestPtr req,
                          std::string const& gw,
                          std::int16_t status,
                          std::string_view body,
                          HeaderAccess hdrs,
                          bool timed_out,
                          std::chrono::system_clock::time_point start) {
  auto req_type = req->type;
  if (req->Finished() ||
      (req->PartiallyRedundant() && req_type == GatewayRequestType::Block)) {
    return;
  }
  auto i = state_.find(gw);
  if (status == 200 || (status == 0)) {
    auto ct = hdrs("content-type");
    auto roots = hdrs("X-Ipfs-Roots");
    std::transform(ct.begin(), ct.end(), ct.begin(), ::tolower);
    ipfs::ipld::BlockSource src;
    src.load_duration = src.fetched_at - start;
    src.cat.gateway_url = gw;
    if (ct.empty()) {
      LOG(ERROR) << "No content-type header?";
    } else if ((!desc.accept.empty()) &&
               ct.find(desc.accept) == std::string::npos) {
      if ((!roots.empty()) && req->type == GatewayRequestType::DnsLink && req->RespondSuccessfully("", api_, src, roots)) {
        LOG(INFO) << "Wrong accept on a DNSLink request, but we still got the resolution.";
        return;
      }
      if (state_.end() != i) {
        i->second.miss(req_type, *req);
        req->failures.insert(gw);
      } else {
        LOG(WARNING) << "No state for " << gw << " to record hit on " << desc.url;
      }
      Next();
      return;
    }
    if (req->RespondSuccessfully(body, api_, src, roots)) {
      if (gw.find("ipfs.eth.aragon.network") < gw.size()) {
        LOG(INFO) << "Success on " << desc.url;
      }
      if (state_.end() != i) {
        i->second.hit(req_type, *req);
      } else {
          LOG(WARNING) << "No state to boost for " << gw;
      }
      Next();
      return;
    }
  }
  req->failures.insert(gw);
  if (status == 408 || status == 504 || status == 429 || status == 110 ||
      timed_out) {
    if (req->type == GatewayRequestType::Block) {
      if (state_.end() != i) {
        i->second.timed_out();
      }
    }
  }
  if (hdrs("X-Ipfs-Path").empty()) {
    i->second.miss(req_type, *req);
  }
  Process(req);
}
