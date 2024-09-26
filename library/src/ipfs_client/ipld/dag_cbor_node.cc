#include "dag_cbor_node.h"
#include <cstdint>
#include <string_view>
#include <memory>
#include <utility>

#include "ipfs_client/ipld/resolution_state.h"
#include "ipfs_client/ipld/dag_node.h"
#include "ipfs_client/response.h"

using Self = ipfs::ipld::DagCborNode;

constexpr std::uint16_t cSuccess = 200U;

auto Self::resolve(ResolutionState& params) -> ResolveResult {
  if (auto cid = doc_->as_link()) {
    auto cid_str = cid.value().to_string();
    return CallChild(params, "", cid_str);
  }
  if (params.IsFinalComponent()) {
    return Response{"text/html", cSuccess, doc_->html(), params.PathToResolve().to_string(), {}};
  }
  return CallChild(params, [this](std::string_view element_name) -> NodePtr {
    if (auto child = doc_->at(element_name)) {
      return std::make_shared<Self>(std::move(child));
    }
    return {};
  });
}

Self::DagCborNode(std::unique_ptr<Data> docptr) : doc_{std::move(docptr)} {}
Self::~DagCborNode() noexcept = default;
