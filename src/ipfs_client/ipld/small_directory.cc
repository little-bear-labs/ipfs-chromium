#include "small_directory.h"

#include <ipfs_client/generated_directory_listing.h>
#include <ipfs_client/ipld/resolution_state.h>
#include <ipfs_client/ipld/dag_node.h>
#include <ipfs_client/response.h>

#include <cstdint>
#include <variant>

using namespace std::literals;

using Self = ipfs::ipld::SmallDirectory;

constexpr std::uint16_t cSuccess = 200U;

auto Self::resolve(ResolutionState& params) -> ResolveResult {
  if (params.IsFinalComponent()) {
    auto result = CallChild(params, "index.html");
    if (auto *resp = std::get_if<Response>(&result)) {
      resp->mime_ = "text/html";
    }
    if (std::get_if<ProvenAbsent>(&result) == nullptr) {
      return result;
    }
    auto dir_path = params.MyPath().to_view();
    GeneratedDirectoryListing index_html{dir_path};
    for (auto& [name, link] : links_) {
      index_html.AddEntry(name);
    }
    return Response{"text/html", cSuccess, index_html.Finish(), "", {}};
  }
  return CallChild(params);
}

Self::~SmallDirectory() noexcept = default;
