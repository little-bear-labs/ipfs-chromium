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
  auto config_idx = 0UL;
  std::vector<std::tuple<long, std::string, GatewayState*>> candidates;
  auto bored = 0U;
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
      //      VLOG(2) << "Not going to resend " << req->debug_string() << " to "
      //              << gw->prefix << " as it has already failed us.";
    } else if (state_iter->second.over_rate(gw->rate)) {
      //      VLOG(2) << "Not considering " << gw->prefix
      //              << " at the moment as it's over its rate limit " <<
      //              gw->rate;
    } else {
      candidates.push_back({state_iter->second.score(*req, gw->rate),
                            gw->prefix, &(state_iter->second)});
      if (state_iter->second.bored()) {
        ++bored;
      }
    }
    std::advance(state_iter, 1);
  }
  if (candidates.empty() && config_idx <= req->failures.size()) {
    LOG(ERROR) << "Request has failed on every gateway I have:"
               << req->debug_string();
    forward(req);
    return false;
  }
  auto to_send = std::max(bored / 2, 3U);
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
  if (req->type == Type::Zombie ||
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
      LOG(WARNING) << "Requested with Accept: " << desc.accept
                   << " but received response with content-type: " << ct;
    } else if (!req->RespondSuccessfully(body, api_)) {
      LOG(ERROR) << "Got an unuseful response from " << gw
                 << " forwarding request " << req->debug_string()
                 << " to next requestor.";
    } else {
      if (state_.end() != i) {
        i->second.hit(*req);
      }
      auto rpm = api_->GetGatewayRate(gw);
      if (rpm < 15) {
        api_->SetGatewayRate(gw, rpm * 2 + 1);
      } else {
        api_->SetGatewayRate(gw, rpm + 1);
      }
      return;
    }
  }
  auto rpm = api_->GetGatewayRate(gw);
  if (status == 408 || status == 504 || status == 429 || status == 110 ||
      timed_out) {
    VLOG(1) << gw << " timed out.";
    if (rpm > 9) {
      api_->SetGatewayRate(gw, rpm - 4);
    } else if (rpm) {
      api_->SetGatewayRate(gw, 0U);
    }
  }
  req->failures.insert(gw);
  if (state_.end() != i && i->second.miss(*req) && rpm) {
    api_->SetGatewayRate(gw, rpm - 1);
  }
  Process(req);
}
