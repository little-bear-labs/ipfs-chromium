#include "gateway_http_requestor.h"

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipld/dag_node.h>

#include <ipfs_client/context_api.h>
#include <ipfs_client/dag_block.h>
#include <ipfs_client/ipns_record.h>
#include <ipfs_client/orchestrator.h>

#include <libp2p/multi/content_identifier_codec.hpp>

#include "log_macros.h"

using Self = ipfs::gw::GatewayHttpRequestor;
using ReqTyp = ipfs::gw::Type;
using CidCodec = libp2p::multi::ContentIdentifierCodec;

std::string_view Self::name() const {
  return "simplistic HTTP requestor";
}
auto Self::handle(ipfs::gw::RequestPtr r) -> HandleOutcome {
  DCHECK(r);
  DCHECK(r->orchestrator);
  DCHECK(r->dependent);
  DCHECK_GT(prefix_.size(), 0UL);
  if (!r->is_http()) {
    LOG(ERROR) << name() << " only handles HTTP requests";
    return HandleOutcome::NOT_HANDLED;
  }
  auto req_key = r->url_suffix().append(r->accept());
  if (seen_.count(req_key)) {
    return HandleOutcome::NOT_HANDLED;
  }
  if (target(*r) <= r->parallel) {
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
  auto cb = [this, r](std::int16_t status, std::string_view body,
                      ContextApi::HeaderAccess) {
    if (r->parallel) {
      r->parallel--;
    }
    if (pending) {
      pending--;
    }
    if (status / 100 == 2) {
      auto nod = node_from_type(r->cid, r->type, body);
      if (nod) {
        if (r->orchestrator->add_node(r->main_param, nod)) {
          r->type = Type::Zombie;
          r->orchestrator->build_response(r->dependent);
        }
        if (typ_good_.insert(r->type).second) {
          LOG(INFO) << prefix_ << " OK with requests of type "
                    << static_cast<int>(r->type);
        } else if (typ_bad_.erase(r->type)) {
          LOG(INFO) << prefix_ << " truly OK with requests of type "
                    << static_cast<int>(r->type);
        }
        if (aff_good_.insert(r->affinity).second) {
          LOG(INFO) << prefix_ << " likes requests in the neighborhood of "
                    << r->affinity;
        } else if (aff_bad_.erase(r->affinity)) {
          LOG(INFO) << prefix_ << " truly OK with affinity " << r->affinity;
        }
        return;
      } else {
        LOG(INFO) << "Got an unuseful response from " << prefix_
                  << " forwarding request " << r->debug_string()
                  << " to next requestor.";
      }
    } else {
      LOG(INFO) << r->debug_string() << " got a failure of status " << status
                << " from " << prefix_;
    }
    aff_bad_.insert(r->affinity);
    typ_bad_.insert(r->type);
    forward(r);
  };
  DCHECK(api_);
  api_->SendHttpRequest(desc.value(), cb);
  seen_.insert(req_key);
  pending++;
  return HandleOutcome::PENDING;
}

Self::GatewayHttpRequestor(std::string gateway_prefix,
                           std::shared_ptr<ContextApi> api)
    : prefix_{gateway_prefix} {
  api_ = api;
}
Self::~GatewayHttpRequestor() {}

ipfs::ipld::NodePtr Self::node_from_type(std::optional<Cid> const& cid,
                                         ReqTyp t,
                                         std::string_view body) const {
  switch (t) {
    case ReqTyp::Block: {
      if (cid.has_value()) {
        ipfs::Block blk{cid.value(), std::string{body}};
        if (blk.valid() && blk.cid_matches_data()) {
          return ipfs::ipld::DagNode::fromBlock(blk);
        }
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
  int result = 0;
  if (pending == 0) {
    result += 1;
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
