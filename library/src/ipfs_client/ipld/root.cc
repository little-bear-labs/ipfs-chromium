#include "root.h"

#include "log_macros.h"

using namespace std::literals;

using Self = ipfs::ipld::Root;
using Ptr = std::shared_ptr<ipfs::ipld::DagNode>;

Self::Root(std::shared_ptr<DagNode> under) {
  links_.push_back({{}, Link{{}, under}});
}
Self::~Root() {}

Ptr Self::deroot() {
  return links_.at(0).second.node;
}
Ptr Self::rooted() {
  return shared_from_this();
}

auto Self::resolve(SlashDelimited path, BlockLookup blu, std::string& to_here)
    -> ResolveResult {
  auto result = deroot()->resolve(path, blu, to_here);
  if (std::get_if<ProvenAbsent>(&result)) {
    auto missing_path = path.to_string();
    if (path.pop() == "_redirects") {
      return result;
    }
    if (!redirects_.has_value()) {
      result = deroot()->resolve("_redirects"sv, blu, to_here);
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
      auto status = redirects_.value().rewrite(missing_path);
      switch (status / 100) {
        case 0:  // no rewrites available
          return result;
        case 2:
          return resolve(std::string_view{missing_path}, blu, to_here);
        case 3:
          // Let the redirect happen
          return Response{"", static_cast<std::uint16_t>(status), "",
                          missing_path};
        case 4:
          result =
              deroot()->resolve(std::string_view{missing_path}, blu, to_here);
          if (std::get_if<ProvenAbsent>(&result)) {
            return Response{"", 500, "", missing_path};
          }
          resp = std::get_if<Response>(&result);
          if (resp) {
            resp->status_ = status;
            return *resp;
          }
          return result;  // MoreDataNeeded to fetch e.g. custom 404 page
        default:
          LOG(ERROR) << "Unsupported status came back from _redirects file: "
                     << status;
      }
    }
    return ProvenAbsent{};
  }
  auto resp = std::get_if<Response>(&result);
  if (resp && resp->location_.empty()) {
    resp->location_ = path.to_string();
  }
  return result;
}
