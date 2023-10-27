#include "ipfs_client/orchestrator.h"

#include <ipfs_client/ipfs_request.h>

#include "ipld/chunk.h"

#include "log_macros.h"
#include "path2url.h"

using namespace std::literals;

using Self = ipfs::Orchestrator;

Self::Orchestrator(GatewayAccess ga, MimeDetection mimer)
    : gw_requestor_{ga}, mimer_{mimer} {}

void Self::build_response(std::shared_ptr<IpfsRequest> req) {
  auto req_path = req->path();
  LOG(INFO) << "build_response(" << req_path.to_string() << ')';
  req_path.pop();  // namespace
  std::string origin{req_path.pop()};
  auto it = dags_.find(origin);
  if (dags_.end() == it) {
    if (gw_request(req, req->path())) {
      build_response(req);
    }
  } else {
    from_tree(req, it->second, req_path);
  }
}
void Self::from_tree(std::shared_ptr<IpfsRequest> req,
                     ipfs::ipld::NodePtr& node,
                     SlashDelimited relative_path) {
  auto root = node->rooted();
  auto block_look_up = [this](auto& k) {
    auto i = dags_.find(k);
    return i == dags_.end() ? ipld::NodePtr{} : i->second;
  };
  auto start = std::string{req->path().pop_n(2)};
  auto result = root->resolve(relative_path, block_look_up, start);
  auto response = std::get_if<Response>(&result);
  if (response) {
    if (response->mime_.empty() && !response->body_.empty()) {
      if (response->location_.empty()) {
        response->mime_ = sniff(req->path(), response->body_);
      } else {
        std::string hit_path{req->path().pop_n(2)};
        if (hit_path.back() != '/' && response->location_.front() != '/') {
          hit_path.push_back('/');
        }
        hit_path.append(response->location_);
        response->mime_ = sniff(SlashDelimited{hit_path}, response->body_);
      }
    }
    // TODO is this necessary? It might be convenient to have this info
    // available, even if it's usually redundant.
    //    if (response->status_ / 100 != 3) {
    //      response->location_.clear();
    //    }
    req->finish(*response);
  } else if (std::get_if<ipld::ProvenAbsent>(&result)) {
    req->finish(Response::PLAIN_NOT_FOUND);
  } else {
    for (auto& path : std::get<ipld::MoreDataNeeded>(result).ipfs_abs_paths_) {
      if (gw_request(req, std::string_view{path})) {
        from_tree(req, node, relative_path);
        return;
      }
    }
  }
}
bool Self::gw_request(std::shared_ptr<IpfsRequest> ir,
                      ipfs::SlashDelimited path) {
  auto req = gw::GatewayRequest::fromIpfsPath(path);
  if (req->type == gw::Type::Identity) {
    auto node =
        std::make_shared<ipld::Chunk>(std::string{req->identity_data()});
    add_node(req->main_param, node);
    return true;
  }
  req->dependent = ir;
  req->orchestrator = shared_from_this();
  gw_requestor_(req);
  if (req->type == gw::Type::Car) {
    gw_request(ir, path.pop_n(2));
  }
  return false;
}

void Self::add_node(std::string key, ipfs::ipld::NodePtr p) {
  if (p) {
    LOG(INFO) << "add_node(" << key << ')';
    dags_[key] = p;
  } else {
    LOG(ERROR) << "NULL block attempted to be added for " << key;
  }
}

std::string Self::sniff(ipfs::SlashDelimited p, std::string const& body) const {
  auto fake_url = path2url(p.to_string());
  auto file_name = p.peek_back();
  auto dot = file_name.find_last_of('.');
  std::string_view ext = "";
  if (dot < file_name.size()) {
    ext = file_name.substr(dot + 1);
  }
  return mimer_(std::string{ext}, body, fake_url);
}
