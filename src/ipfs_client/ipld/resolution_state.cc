#include <ipfs_client/ipld/resolution_state.h>

#include <ipfs_client/client.h>
#include "vocab/slash_delimited.h"
#include "ipfs_client/response_semantic.h"

#include <utility>
#include <string>
#include <string_view>

using Self = ipfs::ipld::ResolutionState;

Self::ResolutionState(SlashDelimited path_to_resolve,
                      ResponseSemantic sem,
                      ipfs::ipld::BlockLookup blu)
    : 
      unresolved_path(path_to_resolve),
      semantic_{sem},
      get_available_block(std::move(blu)) {}

auto Self::IsFinalComponent() const -> bool {
  return !unresolved_path;
}
auto Self::PathToResolve() const -> SlashDelimited {
  return unresolved_path;
}
auto Self::MyPath() const -> SlashDelimited {
  return SlashDelimited{resolved_path_components};
}
auto Self::NextComponent(Client* api) const -> std::string {
  auto copy = unresolved_path;
  if (api != nullptr) {
    return api->UnescapeUrlComponent(copy.pop());
  } else {
    return std::string{copy.pop()};
  }
}
auto Self::GetBlock(std::string const& block_key) const -> NodePtr {
  return get_available_block(block_key);
}
auto Self::WithPath(std::string_view new_path) const -> Self {
  auto result = *this;
  result.unresolved_path = SlashDelimited{new_path};
  return result;
}
auto Self::RestartResolvedPath() const -> ResolutionState {
  auto newme = *this;
  newme.resolved_path_components.clear();
  return newme;
}
void Self::Descend() {
  auto next = unresolved_path.pop();
  if (next.empty()) {
    return;
  }
  if (!resolved_path_components.ends_with('/')) {
    resolved_path_components.push_back('/');
  }
  resolved_path_components.append(next);
}
auto Self::Semantic() const -> ResponseSemantic {
  return semantic_;
}
