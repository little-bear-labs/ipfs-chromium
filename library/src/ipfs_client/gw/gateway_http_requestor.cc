#include "gateway_http_requestor.h"

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipld/dag_node.h>

#include <ipfs_client/context_api.h>
#include <ipfs_client/ipns_record.h>
#include <ipfs_client/orchestrator.h>
#include <ipfs_client/pb_dag.h>

#include "log_macros.h"

using Self = ipfs::gw::GatewayHttpRequestor;
using ReqTyp = ipfs::gw::Type;

std::string_view Self::name() const {
  return "simplistic HTTP requestor";
}
auto Self::handle(ipfs::gw::RequestPtr r) -> HandleOutcome {
  DCHECK(r);
  DCHECK(r->dependent);
  DCHECK_GT(prefix_.size(), 0UL);
  if (!r->is_http()) {
    LOG(ERROR) << name() << " only handles HTTP requests";
    return HandleOutcome::NOT_HANDLED;
  }
  auto req_key = r->url_suffix().append(r->accept());
  if (seen_[req_key] > 0xFD) {
    return HandleOutcome::NOT_HANDLED;
  }
  if (target(*r) <= r->parallel + pending_ + seen_[req_key]) {
    return HandleOutcome::MAYBE_LATER;
  }
  auto desc = r->describe_http();
  if (!desc.has_value() || desc.value().url.empty()) {
    LOG(ERROR)
        << r->debug_string()
        << " is HTTP but can't describe the HTTP request that would happen?";
    return HandleOutcome::NOT_HANDLED;
  }
  if (prefix_.back() == '/' && desc.value().url[0] == '/') {
    desc.value().url.insert(0, prefix_, 0UL, prefix_.size() - 1UL);
  } else {
    desc.value().url.insert(0, prefix_);
  }
  desc.value().timeout_seconds += extra_seconds_;
  auto cb = [this, r, desc, req_key](std::int16_t status, std::string_view body,
                                     ContextApi::HeaderAccess ha) {
    if (r->parallel) {
      r->parallel--;
    }
    if (pending_) {
      pending_--;
    }
    if (r->type == Type::Zombie) {
      return;
    } else if (status == 408 || status == 504) {
      // Timeouts
      extra_seconds_++;
      forward(r);
      return;
    } else if (status / 100 == 2) {
      auto ct = ha("content-type");
      std::transform(ct.begin(), ct.end(), ct.begin(), ::tolower);
      if (ct.empty()) {
        LOG(ERROR) << "No content-type header?";
      }
      if (ct.size() && desc->accept.size() &&
          ct.find(desc->accept) == std::string::npos) {
        LOG(WARNING) << "Requested with Accept: " << desc->accept
                     << " but received response with content-type: " << ct;
        LOG(INFO) << "Demote(" << prefix_ << ')';
      } else if (!r->RespondSuccessfully(body, api_)) {
        LOG(ERROR) << "Got an unuseful response from " << prefix_
                   << " forwarding request " << r->debug_string()
                   << " to next requestor.";
      } else {
        // Good cases
        if (typ_good_.insert(r->type).second) {
          VLOG(1) << prefix_ << " OK with requests of type "
                  << static_cast<int>(r->type);
        } else if (typ_bad_.erase(r->type)) {
          VLOG(1) << prefix_ << " truly OK with requests of type "
                  << static_cast<int>(r->type);
        }
        if (aff_good_.insert(r->affinity).second) {
          VLOG(1) << prefix_ << " likes requests in the neighborhood of "
                  << r->affinity;
        } else if (aff_bad_.erase(r->affinity)) {
          VLOG(1) << prefix_ << " truly OK with affinity " << r->affinity;
        }
        VLOG(2) << prefix_ << " had a success on " << r->debug_string();
        LOG(INFO) << "Promote(" << prefix_ << ')';
        ++strength_;
        return;
      }
    } else if (status / 100 == 4) {
      seen_[req_key] += 9;
    }
    seen_[req_key] += 9;
    LOG(INFO) << "Demote(" << prefix_ << ')';
    if (strength_ > 0) {
      --strength_;
    }
    aff_bad_.insert(r->affinity);
    typ_bad_.insert(r->type);
    forward(r);
  };
  DCHECK(api_);
  api_->SendHttpRequest(desc.value(), cb);
  seen_[req_key]++;
  pending_++;
  return HandleOutcome::PENDING;
}

Self::GatewayHttpRequestor(std::string gateway_prefix,
                           int strength,
                           std::shared_ptr<ContextApi> api)
    : prefix_{gateway_prefix}, strength_{strength} {
  api_ = api;
}
Self::~GatewayHttpRequestor() {}

ipfs::ipld::NodePtr Self::node_from_type(std::optional<Cid> const& cid,
                                         ReqTyp t,
                                         std::string_view body) const {
  switch (t) {
    case ReqTyp::Block: {
      if (cid.has_value()) {
        ipfs::PbDag blk{cid.value(), as_bytes(body)};
        if (blk.cid_matches_data(*api_)) {
          return ipfs::ipld::DagNode::fromBlock(blk);
        }
      } else {
        LOG(ERROR) << "Block request on an invalid CID.";
      }
      return {};
    }
    case ReqTyp::Ipns: {
      if (cid.has_value()) {
        auto byte_ptr = reinterpret_cast<ipfs::Byte const*>(body.data());
        auto rec = ipfs::ValidateIpnsRecord({byte_ptr, body.size()},
                                            cid.value(), *api_);
        if (rec.has_value()) {
          return ipfs::ipld::DagNode::fromIpnsRecord(rec.value());
        } else {
          LOG(ERROR) << "IPNS record failed to validate!";
        }
      }
      return {};
    }
    case ReqTyp::Identity:
      LOG(ERROR) << "An HTTP response from a gateway received for an identity "
                    "(inlined) CID";
      return {};
    case ReqTyp::DnsLink:
      LOG(WARNING) << "HTTP responses to DnsLink requests not yet implemented, "
                      "and it's not clear they will be.";
      return {};
    case ReqTyp::Car:
      LOG(INFO) << "TODO responses to Car requests not yet implemented.";
      return {};
    case ReqTyp::Providers:
      LOG(INFO) << "TODO responses to Car requests not yet implemented: "
                << body;
      return {};
    case ReqTyp::Zombie:
      return {};
  }
  return {};  // TODO
}
int Self::target(GatewayRequest const& r) const {
  int result = (strength_ - pending_) / 2;
  if (!pending_) {
    ++result;
  }
  if (typ_good_.count(r.type)) {
    result += 3;
  }
  if (!typ_bad_.count(r.type)) {
    result += 2;
  }
  if (aff_good_.count(r.affinity)) {
    result += 5;
  }
  if (aff_bad_.count(r.affinity) == 0UL) {
    result += 4;
  }
  return result;
}
