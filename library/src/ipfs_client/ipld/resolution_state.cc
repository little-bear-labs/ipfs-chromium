#include <ipfs_client/ipld/resolution_state.h>

#include <ipfs_client/context_api.h>

using Self = ipfs::ipld::ResolutionState;

bool Self::IsFinalComponent() const {
  return !unresolved_path;
}
auto Self::PathToResolve() const -> SlashDelimited {
  return unresolved_path;
}
auto Self::MyPath() const -> SlashDelimited {
  return SlashDelimited{resolved_path_components};
}
std::string Self::NextComponent(ContextApi const* api) const {
  auto copy = unresolved_path;
  if (api) {
    return api->UnescapeUrlComponent(copy.pop());
  } else {
    return std::string{copy.pop()};
  }
}
auto Self::GetBlock(std::string const& block_key) const -> NodePtr {
  return get_available_block(block_key);
}
Self Self::WithPath(std::string_view p) const {
  auto rv = *this;
  rv.unresolved_path = SlashDelimited{p};
  return rv;
}
auto Self::RestartResolvedPath() const -> ResolutionState {
  auto rv = *this;
  rv.resolved_path_components.clear();
  return rv;
}