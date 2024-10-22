#include "unixfs_file.h"
#include <cstdint>
#include <vector>
#include <string>
#include <variant>

#include "ipfs_client/ipld/resolution_state.h"
#include "ipfs_client/ipld/dag_node.h"
#include "ipfs_client/response.h"
#include "log_macros.h"

using namespace std::literals;

using Self = ipfs::ipld::UnixfsFile;

constexpr std::uint16_t cSuccess = 200U;

auto Self::resolve(ResolutionState& params) -> ResolveResult {
  if (!params.IsFinalComponent()) {
    LOG(ERROR) << "Can't path through a file, (at " << params.MyPath()
               << ") but given the path " << params.PathToResolve();
    return ProvenAbsent{};
  }
  std::vector<std::string> missing;
  std::string body;
  for (auto& child : links_) {
    auto& link = child.second;
    if (!link.node) {
      link.node = params.GetBlock(link.cid);
    }
    if (link.node) {
      auto recurse = link.node->Resolve(params);
      auto *mdn = std::get_if<MoreDataNeeded>(&recurse);
      if (mdn != nullptr) {
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
        cSuccess,
        body,
        params.MyPath().to_string(), {},
    };
  }
  auto result = MoreDataNeeded{missing};
  return result;
}

Self::~UnixfsFile() noexcept = default;
