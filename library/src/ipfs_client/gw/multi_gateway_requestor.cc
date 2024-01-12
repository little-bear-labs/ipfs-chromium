#include "multi_gateway_requestor.h"

#include <ipfs_client/gw/gateway_request.h>

#include "log_macros.h"

#include <algorithm>
#include <chrono>

using Self = ipfs::gw::MultiGatewayRequestor;

namespace ch = std::chrono;

std::string_view Self::name() const {
  return "multi-gateway requestor";
}
auto Self::handle(RequestPtr r) -> HandleOutcome {
  if (!r->is_http()) {
    LOG(INFO) << r->debug_string() << " is not an HTTP request.";
    return HandleOutcome::NOT_HANDLED;
  }
  if (!q.empty()) {
    auto popped = q.front();
    q.pop_front();
    Process(popped);
  }
  Process(r);
  return HandleOutcome::PENDING;
}
bool Self::Process(RequestPtr const& req) {
  if (!req->is_http()) {
    return false;
  }
  auto state_iter = state_.begin();
  auto config_idx = 0U;
  using Candidate = std::tuple<long, std::string, GatewayState*>;
  std::vector<Candidate> candidates;
  auto bored = 0U;
  Candidate over_rate = {0, {}, nullptr};
  while (auto gw = api_->GetGateway(config_idx++)) {
    if (state_iter == state_.end() || state_iter->first > gw->prefix) {
      VLOG(2) << "A new gateway has entered the chat: " << gw->prefix << '='
              << gw->rate;
      // One can insert like this because state_ is std::map w/ stable iterators
      state_iter = state_.insert({gw->prefix, GatewayState{}}).first;
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
    } else if (state_iter->second.over_rate(gw->rate)) {
      auto score = state_iter->second.score(*req, gw->rate);
      if (std::get<0>(over_rate) <= score) {
        over_rate = std::make_tuple(score, gw->prefix, &(state_iter->second));
      }
    } else {
      candidates.push_back({state_iter->second.score(*req, gw->rate),
                            gw->prefix, &(state_iter->second)});
      if (state_iter->second.bored()) {
        ++bored;
      }
    }
    std::advance(state_iter, 1);
  }
  if (std::get<2>(over_rate)) {
    if (candidates.empty()) {
      VLOG(2) << "Overburdened.";
    }
    candidates.push_back(over_rate);
  }
  if (candidates.empty() && config_idx <= req->failures.size()) {
    LOG(ERROR) << "Run out of gateways to send this to:" << req->debug_string();
    forward(req);
    return false;
  }
  auto to_send = std::max(bored / 2UL, 2UL);
  std::sort(candidates.begin(), candidates.end(), std::greater{});
  for (auto& [score, prefix, state] : candidates) {
    DoSend(req, prefix, *state);
    if (!--to_send) {
      return true;
    }
  }
  q.push_back(req);
  return false;
}
void Self::DoSend(RequestPtr req, std::string const& gw, GatewayState& state) {
  auto desc = req->describe_http(gw);
  if (!desc.has_value()) {
    LOG(ERROR) << "A request that has no HTTP description got pretty far "
                  "toward doing an HTTP request: "
               << req->debug_string();
    return;
  }
  if (state.extra_ms()) {
    desc->timeout_seconds += state.extra_ms() / 1000L + 1L;
  }
  auto timeout_threshold =
      ch::system_clock::now() +
      ch::seconds(desc->timeout_seconds ? desc->timeout_seconds : 300) -
      ch::milliseconds(1);
  auto hold_alive = shared_from_this();
  auto cb = [this, hold_alive, req, gw, timeout_threshold, desc](
                std::int16_t s, std::string_view b, auto h) {
    auto timed_out = ch::system_clock::now() >= timeout_threshold;
    HandleResponse(*desc, req, gw, s, b, h, timed_out);
  };
  state.just_sent_one();
  api_->SendHttpRequest(*desc, cb);
}
void Self::HandleResponse(HttpRequestDescription const& desc,
                          RequestPtr req,
                          std::string const& gw,
                          std::int16_t status,
                          std::string_view body,
                          ContextApi::HeaderAccess hdrs,
                          bool timed_out) {
  if (req->Finished() ||
      (req->PartiallyRedundant() && req->type == Type::Block)) {
    return;
  }
  auto i = state_.find(gw);
  if (status == 200) {
    auto ct = hdrs("content-type");
    std::transform(ct.begin(), ct.end(), ct.begin(), ::tolower);
    if (ct.empty()) {
      LOG(ERROR) << "No content-type header?";
    } else if (desc.accept.size() &&
               ct.find(desc.accept) == std::string::npos) {
      VLOG(2) << "Requested with Accept: " << desc.accept
              << " but received response with content-type: " << ct;
      return;
    }
    if (!req->RespondSuccessfully(body, api_)) {
      VLOG(1) << "Got an unuseful response from " << gw << " for request "
              << req->debug_string();
    } else {
      VLOG(2) << "Response from " << gw << " to " << req->debug_string()
              << " was successful & useful - progress made.";
      if (state_.end() != i) {
        i->second.hit(*req);
      }
      auto rpm = api_->GetGatewayRate(gw);
      VLOG(2) << "Rate for " << gw << " _WAS_ " << rpm
              << " and is about to go up.";
      if (rpm < 15) {
        rpm *= 2;
      }
      rpm += 2;
      api_->SetGatewayRate(gw, rpm);
      return;
    }
  }
  auto rpm = api_->GetGatewayRate(gw);
  auto old_rpm = rpm;
  if (status == 408 || status == 504 || status == 429 || status == 110 ||
      timed_out) {
    LOG(WARNING) << gw << " timed out on request " << req->debug_string();
    if (req->type == Type::Block) {
      if (state_.end() != i) {
        i->second.timed_out();
      }
      if (rpm > 60U) {
        rpm -= 9;
      } else if (rpm) {
        --rpm;
      }
    }
  } else {
    req->failures.insert(gw);
  }
  if (state_.end() != i && i->second.miss(*req) && rpm &&
      req->type == Type::Block) {
    --rpm;
  }
  if (old_rpm != rpm) {
    api_->SetGatewayRate(gw, rpm);
  }
  Process(req);
}
