#include "ipfs_client/block_storage.h"

using Self = ipfs::BlockStorage;
using Resolution = Self::Resolution;
using Node = ipfs::Block;

bool Self::Store(std::string&& cid, Node&& block) {
  // TODO validate
  cid2node_.emplace(std::move(cid), std::move(block));
  return true;
}
auto Self::Get(std::string const& cid) const -> Node const* {
  auto it = cid2node_.find(cid);
  if (it == cid2node_.end()) {
    return nullptr;
  }
  return &(it->second);
}
Resolution Self::AttemptResolve(std::string const& root,
                                std::string_view path) const {
  Resolution result;
  AttemptResolveInner(result, root, path);
  return result;  // TODO
}
void Self::AttemptResolveInner(Resolution& result,
                               std::string const& root,
                               std::string_view path) const {
  Node const* node = Get(root);
  if (!node) {
    result.required_cids.push_back(root);
    return;
  }
  auto slash_count = path.find_first_not_of("/");
  if (slash_count <= path.size()) {
    path.remove_prefix(slash_count);
  }
  auto slash = path.find('/');
  auto path_element = path.substr(0, slash);
  if (path_element.empty()) {
    // TODO we're at the leaf of the path
  }
}

Self::BlockStorage() {}
Self::~BlockStorage() noexcept {}
Self::Resolution::Resolution() {}
Self::Resolution::~Resolution() noexcept {}
