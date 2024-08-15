#include "root.h"

#include "log_macros.h"

using namespace std::literals;

using Self = ipfs::ipld::Root;
using Ptr = std::shared_ptr<ipfs::ipld::DagNode>;

Self::Root(std::shared_ptr<DagNode> under) {
  links_.push_back({{}, Link{{}, under}});
}
Self::~Root() noexcept = default;

Ptr Self::deroot() {
  return links_.at(0).second.node;
}
Ptr Self::rooted() {
  return shared_from_this();
}
bool Self::expired() const {
  auto n = links_.at(0).second.node;
  return n ? n->expired() : true;
}

auto Self::resolve(ResolutionState& params) -> ResolveResult {
  auto location = params.PathToResolve().to_string();
  auto result = deroot()->Resolve(params);
  params.headers.Finish();
  if (auto pc = std::get_if<PathChange>(&result)) {
    auto lower = params.WithPath(pc->new_path);
    result = resolve(lower);
    location.assign(lower.MyPath().to_view());
  } else if (std::get_if<ProvenAbsent>(&result)) {
    if (params.NextComponent(api_.get()) == "_redirects") {
      return result;
    }
    if (!redirects_.has_value()) {
      auto redirects_path = params.WithPath("_redirects");
      result = resolve(redirects_path);
      auto redirect_resp = std::get_if<Response>(&result);
      if (redirect_resp && redirect_resp->status_ == 200) {
        redirects_ = redirects::File(redirect_resp->body_);
      } else {
        // Either this is ProvenAbsent, in which case this will be interpreted
        // as the original ProvenAbsent Or it's MoreDataNeeded but for
        // _redirects, which is what we need now
        return result;
      }
    }
    if (redirects_.has_value() && redirects_.value().valid()) {
      Response* resp = nullptr;
      auto status = redirects_.value().rewrite(location);
      if (location.find("://") < location.size()) {
        VLOG(2) << "_redirects file sent us to a whole URL, scheme-and-all: "
                << location << " status=" << status;
        return Response{"", status, "", location, params.headers};
      }
      auto lower_parm = params.WithPath(location).RestartResolvedPath();
      switch (status / 100) {
        case 0:  // no rewrites available
          break;
        case 2:
          result = deroot()->Resolve(lower_parm);
          location.assign(lower_parm.MyPath().to_view());
          break;
        case 3:
          // Let the redirect happen
          return Response{"", status, "", location, params.headers};
        case 4: {
          result = deroot()->Resolve(lower_parm);
          location.assign(lower_parm.MyPath().to_view());
          if (std::get_if<ProvenAbsent>(&result)) {
            return Response{"", 500, "", location, params.headers};
          }
          resp = std::get_if<Response>(&result);
          if (resp) {
            resp->status_ = status;
            resp->headers_ = params.headers;
            return *resp;
          }
          break;  // MoreDataNeeded to fetch e.g. custom 404 page
        }
        default:
          LOG(ERROR) << "Unsupported status came back from _redirects file: "
                     << status;
          return ProvenAbsent{};
      }
    }
  }
  auto resp = std::get_if<Response>(&result);
  if (resp) {
    if (resp->location_.empty()) {
      resp->location_ = location;
    }
    resp->headers_ = params.headers;
  }
  return result;
}
