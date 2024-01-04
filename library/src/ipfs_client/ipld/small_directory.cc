#include "small_directory.h"

#include <ipfs_client/context_api.h>
#include "ipfs_client/generated_directory_listing.h"
#include "ipfs_client/path2url.h"

#include "log_macros.h"

#include <numeric>

using namespace std::literals;

using Self = ipfs::ipld::SmallDirectory;

auto Self::resolve(ResolutionState& params) -> ResolveResult {
  if (params.IsFinalComponent()) {
    VLOG(2) << "Directory listing requested for " << params.MyPath();
    auto result = CallChild(params, "index.html");
    if (auto resp = std::get_if<Response>(&result)) {
      resp->mime_ = "text/html";
    }
    if (!std::get_if<ProvenAbsent>(&result)) {
      return result;
    }
    auto dir_path = params.MyPath().to_view();
    GeneratedDirectoryListing index_html{dir_path};
    for (auto& [name, link] : links_) {
      index_html.AddEntry(name);
    }
    return Response{"text/html", 200, index_html.Finish(), ""};
  }
  return CallChild(params);
}

Self::~SmallDirectory() {}
