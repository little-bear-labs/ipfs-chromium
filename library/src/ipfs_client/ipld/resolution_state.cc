#include <ipfs_client/ipld/resolution_state.h>

#include <ipfs_client/client.h>

using Self = ipfs::ipld::ResolutionState;

Self::ResolutionState(std::string_view path, ipfs::ipld::BlockLookup blu)
    : ResolutionState(SlashDelimited{path}, blu) {}
Self::ResolutionState(SlashDelimited path_to_resolve,
                      ipfs::ipld::BlockLookup blu)
    : resolved_path_components{},
      unresolved_path(path_to_resolve),
      get_available_block(blu) {}

bool Self::IsFinalComponent() const {
  return !unresolved_path;
}
auto Self::PathToResolve() const -> SlashDelimited {
  return unresolved_path;
}
auto Self::MyPath() const -> SlashDelimited {
  return SlashDelimited{resolved_path_components};
}
std::string Self::NextComponent(Client* api) const {
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
