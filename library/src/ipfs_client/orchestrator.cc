#include "ipfs_client/orchestrator.h"

#include <ipfs_client/context_api.h>
#include <ipfs_client/gateways.h>
#include <ipfs_client/ipfs_request.h>

#include "log_macros.h"
#include "path2url.h"

using namespace std::literals;

using Self = ipfs::Orchestrator;

Self::Orchestrator(GatewayAccess ga,
                   std::shared_ptr<gw::Requestor> requestor,
                   std::shared_ptr<ContextApi> api)
    : gw_requestor_{ga}, api_{api}, requestor_{requestor} {
  DCHECK(requestor);
}

void Self::build_response(std::shared_ptr<IpfsRequest> req) {
  if (!req->ready_after()) {
    return;
  }
  auto req_path = req->path();
  LOG(INFO) << "build_response(" << req_path.to_string() << ')';
  req_path.pop();  // namespace
  std::string affinity{req_path.pop()};
  auto it = dags_.find(affinity);
  if (dags_.end() == it) {
    if (gw_request(req, req->path(), affinity)) {
      build_response(req);
    }
  } else {
    LOG(INFO) << "Requesting root " << affinity << " resolve path "
              << req_path.to_string();
    from_tree(req, it->second, req_path, affinity);
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
    req->finish(*response);
  } else if (std::get_if<ipld::ProvenAbsent>(&result)) {
    req->finish(Response::PLAIN_NOT_FOUND);
  } else {
    auto& missing_paths =
        std::get<ipld::MoreDataNeeded>(result).ipfs_abs_paths_;
    req->till_next(missing_paths.size());
    bool repeat = false;
    for (auto& path : missing_paths) {
      if (gw_request(req, std::string_view{path}, affinity)) {
        repeat = true;
      }
    }
    if (repeat) {
      from_tree(req, node, relative_path, affinity);
    }
  }
}
bool Self::gw_request(std::shared_ptr<IpfsRequest> ir,
                      ipfs::SlashDelimited path,
                      std::string const& aff) {
  auto req = gw::GatewayRequest::fromIpfsPath(path);
  req->dependent = ir;
  req->orchestrator = shared_from_this();
  req->affinity = aff;
  //  gw_requestor_(req);
  requestor_->request(req);
  return false;
}

bool Self::add_node(std::string key, ipfs::ipld::NodePtr p) {
  if (p) {
    LOG(INFO) << "add_node(" << key << ')';
    if (dags_.insert({key, p}).second) {
      p->set_api(api_);
      return true;
    }
  } else {
    LOG(ERROR) << "NULL block attempted to be added for " << key;
  }
  return false;
}

std::string Self::sniff(ipfs::SlashDelimited p, std::string const& body) const {
  auto fake_url = path2url(p.to_string());
  auto file_name = p.peek_back();
  auto dot = file_name.find_last_of('.');
  std::string ext = "";
  if (dot < file_name.size()) {
    ext.assign(file_name, dot + 1);
  }
  auto result = api_->MimeType(ext, body, fake_url);
  LOG(INFO) << "Deduced mime from (ext=" << ext << " body of " << body.size()
            << " bytes, 'url'=" << fake_url << ")=" << result;
  return result;
}

bool Self::has_key(std::string const& k) const {
  return dags_.count(k);
}