#include "ipfs_client/partition.h"

#include <ipfs_client/client.h>
#include <ipfs_client/ipfs_request.h>

#include "ipfs_client/gw/requestor.h"
#include "ipfs_client/ipld/dag_node.h"
#include "ipfs_client/ipld/resolution_state.h"
#include "ipfs_client/gw/gateway_request.h"
#include "log_macros.h"
#include "path2url.h"
#include "vocab/slash_delimited.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <string>
#include <variant>

using namespace std::literals;

using Self = ipfs::Partition;

Self::Partition(std::shared_ptr<gw::Requestor> requestor,
                std::shared_ptr<Client> api)
    : api_{std::move(api)}, requestor_{requestor} {
  DCHECK(requestor);
}

void Self::build_response(std::shared_ptr<IpfsRequest> req) {
  if (!req) {
    LOG(ERROR) << "Can't build a response to NULL.";
    return;
  }
  auto req_path = req->path();
  req_path.pop();  // discard namespace ipfs or ipns
  std::string const affinity{req_path.pop()};
  auto it = dags_.find(affinity);
  if (dags_.end() == it) {
    if (gw_request(req, req->path(), affinity)) {
      build_response(req);
    }
  } else {
    auto root = it->second->rooted();
    if (root != it->second) {
      it->second = root;
    }
    from_tree(req, root, req_path, affinity);
  }
}
void Self::from_tree(std::shared_ptr<IpfsRequest> req,
                     ipfs::ipld::NodePtr& node,
                     SlashDelimited relative_path,
                     std::string const& affinity) {
  auto root = node->rooted();
  auto block_look_up = [this](auto& k) {
    auto i = dags_.find(k);
    return i == dags_.end() ? ipld::NodePtr{} : i->second;
  };
  auto start = std::string{req->path().pop_n(2)};
  ipfs::ipld::ResolutionState state{relative_path, req->semantic(), block_look_up};
  auto result = root->Resolve(state);
  auto *response = std::get_if<Response>(&result);
  if (response != nullptr) {
    if (response->mime_.empty() && !response->body_.empty()) {
      if (response->location_.empty()) {
        response->mime_ = sniff(req->path(), response->body_);
      } else {
        std::string hit_path{req->path().pop_n(2)};
        if (!hit_path.ends_with('/') &&
            !(response->location_.starts_with('/'))) {
          hit_path.push_back('/');
        }
        hit_path.append(response->location_);
        response->mime_ = sniff(SlashDelimited{hit_path}, response->body_);
      }
    }
    if (response->status_ / 100 != 3) {
      response->location_.clear();
    }
    req->finish(*response);
    if (node->expired()) {
      dags_.erase(affinity);
    }
  } else if (auto* pc = std::get_if<ipld::PathChange>(&result)) {
    LOG(ERROR) << "Should not be getting a PathChange in orchestrator - "
                  "should've been handled in the root, but got "
               << pc->new_path << " for " << req->path();
  } else if (std::get_if<ipld::ProvenAbsent>(&result) != nullptr) {
    req->finish(Response::IMMUTABLY_GONE);
  } else {
    auto& mps = std::get<ipld::MoreDataNeeded>(result).ipfs_abs_paths_;
    if (std::any_of(mps.begin(), mps.end(), [this, &req, &affinity](auto& p) {
          return gw_request(req, SlashDelimited{p}, affinity);
        })) {
      from_tree(req, node, relative_path, affinity);
    }
  }
}
auto Self::gw_request(std::shared_ptr<IpfsRequest> ir,
                      ipfs::SlashDelimited path,
                      std::string const& aff) -> bool {
  auto req = gw::GatewayRequest::fromIpfsPath(path);
  if (req) {
    req->dependent = ir;
    req->orchestrator(shared_from_this());
    req->affinity = aff;
    requestor_->request(req);
  } else {
    LOG(ERROR) << "Failed to create a request for " << path.to_string();
  }
  return false;
}

auto Self::add_node(std::string key, ipfs::ipld::NodePtr p) -> bool {
  if (!p) {
    LOG(INFO) << "NULL block attempted to be added for " << key;
    return false;
  }
  auto [it, first] = dags_.insert({key, p});
  if (first) {
    // VLOG(2) << "First node showed up for [" << key << "].";
  } else if (p->PreferOver(*it->second)) {
    it->second = p;
  } else {
    return false;
  }
  p->set_api(api_);
  return true;
}

auto Self::sniff(ipfs::SlashDelimited p, std::string const& body) const -> std::string {
  auto fake_url = path2url(p.to_string());
  auto file_name = p.peek_back();
  auto dot = file_name.find_last_of('.');
  std::string ext;
  if (dot < file_name.size()) {
    ext.assign(file_name, dot + 1);
  }
  auto result = api_->MimeType(ext, body, fake_url);
  return result;
}

auto Self::has_key(std::string const& k) const -> bool {
  return dags_.count(k) != 0U;
}
