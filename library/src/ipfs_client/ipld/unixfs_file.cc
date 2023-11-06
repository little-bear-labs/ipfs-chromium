#include "unixfs_file.h"

#include "log_macros.h"

using namespace std::literals;

using Self = ipfs::ipld::UnixfsFile;

auto Self::resolve(ipfs::SlashDelimited path,
                   ipfs::ipld::DagNode::BlockLookup blu,
                   std::string& to_here) -> ResolveResult {
  if (path) {
    LOG(ERROR) << "Can't path through a file, (at " << to_here
               << ") but given the path " << path.to_string();
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
      LOG(INFO) << "In order to resolve the file at path " << to_here
                << " I need CID " << link.cid;
      missing.push_back("/ipfs/" + link.cid);
    }
  }
  if (missing.empty()) {
    LOG(INFO) << "Assembled file (or file part) of " << body.size()
              << " bytes from multiple nodes.";
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
