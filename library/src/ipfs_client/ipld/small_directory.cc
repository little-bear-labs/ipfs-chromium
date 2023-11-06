#include "small_directory.h"

#include <ipfs_client/context_api.h>
#include "ipfs_client/generated_directory_listing.h"
#include "ipfs_client/path2url.h"

#include "log_macros.h"

using namespace std::literals;

using Self = ipfs::ipld::SmallDirectory;

namespace {
ipfs::ipld::NodePtr& node(ipfs::ipld::Link& l,
                          ipfs::ipld::DagNode::BlockLookup f) {
  if (!l.node) {
    l.node = f(l.cid);
  }
  return l.node;
}
}  // namespace

auto Self::resolve(SlashDelimited path, BlockLookup blu, std::string& to_here)
    -> ResolveResult {
  if (!path) {
    LOG(INFO) << "Directory listing requested.";
    GeneratedDirectoryListing index_html{to_here};
    for (auto& [name, link] : links_) {
      LOG(INFO) << "Listing " << to_here << " encountered " << name << '='
                << link.cid;
      if (name == "index.html") {
        auto& n = node(link, blu);
        if (n) {
          auto result = n->resolve(""sv, blu, to_here.append("/index.html"));
          auto resp = std::get_if<Response>(&result);
          if (resp) {
            resp->mime_ = "text/html";
          }
          return result;
        } else {
          return MoreDataNeeded{std::vector{"/ipfs/" + link.cid}};
        }
      } else {
        index_html.AddEntry(name);
      }
    }
    return Response{"text/html", 200, index_html.Finish(), ""};
  }
  auto name = api_->UnescapeUrlComponent(path.pop());
  LOG(INFO) << "Looking for '" << name << "' in directory.";
  // TODO binary search
  auto match = [&name](auto& l) { return l.first == name; };
  auto it = std::find_if(links_.begin(), links_.end(), match);
  if (links_.end() == it) {
    LOG(INFO) << name << " does not exist in directory.";
    return ProvenAbsent{};
  }
  auto& link = it->second;
  auto& nod = node(link, blu);
  if (nod) {
    if (to_here.back() != '/') {
      to_here.push_back('/');
    }
    to_here.append(name);
    LOG(INFO) << "Descending into " << link.cid << " for " << name;
    return nod->resolve(path, blu, to_here);
  } else {
    LOG(INFO) << "Should descending into " << link.cid << " for " << name
              << " but can't because it's missing. Will request.";
    return MoreDataNeeded{std::vector{"/ipfs/" + link.cid}};
  }
}

Self::~SmallDirectory() {}
