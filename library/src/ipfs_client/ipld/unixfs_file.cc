#include "unixfs_file.h"

using namespace std::literals;

using Self = ipfs::ipld::UnixfsFile;

auto Self::resolve(ipfs::SlashDelimited path,
                   ipfs::ipld::DagNode::BlockLookup blu,
                   std::string& to_here) -> ResolveResult {
  if (path) {
    // You can't path through a file.
    return ProvenAbsent{};
  }
  std::vector<std::string> missing;
  std::string body;
  for (auto& child : links_) {
    auto& link = child.second;
    if (!link.node) {
      link.node = blu(link.cid);
    }
    if (link.node) {
      auto recurse = link.node->resolve(""sv, blu, to_here);
      auto mdn = std::get_if<MoreDataNeeded>(&recurse);
      if (mdn) {
        missing.insert(missing.end(), mdn->ipfs_abs_paths_.begin(),
                       mdn->ipfs_abs_paths_.end());
        continue;
      }
      if (missing.empty()) {
        body.append(std::get<Response>(recurse).body_);
      }
    } else {
      missing.push_back("/ipfs/" + link.cid);
    }
  }
  if (missing.empty()) {
    return Response{
        "",
        200,
        body,
        ""s,
    };
  }
  return MoreDataNeeded{missing};
}

Self::~UnixfsFile() {}
