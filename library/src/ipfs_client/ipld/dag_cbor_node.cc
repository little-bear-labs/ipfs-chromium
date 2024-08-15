#include "dag_cbor_node.h"

#include "log_macros.h"

using Self = ipfs::ipld::DagCborNode;

auto Self::resolve(ResolutionState& params) -> ResolveResult {
  if (auto cid = doc_->as_link()) {
    auto cid_str = cid.value().to_string();
    return CallChild(params, "", cid_str);
  }
  if (params.IsFinalComponent()) {
    return Response{"text/html", 200, doc_->html(), params.PathToResolve().to_string(), {}};
  }
  return CallChild(params, [this](std::string_view element_name) -> NodePtr {
    if (auto child = doc_->at(element_name)) {
      return std::make_shared<Self>(std::move(child));
    }
    return {};
  });
}

Self::DagCborNode(std::unique_ptr<Data> p) : doc_{std::move(p)} {}
Self::~DagCborNode() noexcept = default;
