#include "dag_cbor_node.h"

#include "log_macros.h"

using Self = ipfs::ipld::DagCborNode;

auto Self::resolve(SlashDelimited path, BlockLookup blu, std::string& to_here) -> ResolveResult {
  if (auto cid = doc_->as_link()) {
    auto cid_str = cid.value().to_string();
    LOG(INFO) << to_here << " I am a DAG-CBOR link to " << cid_str;
    if (links_.empty()) {
      Link l{cid_str, blu(cid_str)};
      links_.push_back({std::string{}, l});
    }
    auto& child = links_.back().second;
    if (!child.node) {
      child.node = blu(child.cid);
    }
    if (child.node) {
      LOG(INFO) << to_here << " Indirection through link... ";
      return child.node->resolve(path, blu, to_here);
    }
    return MoreDataNeeded{{"/ipfs/"+child.cid}};
  }
  if (!path) {
    LOG(INFO) << to_here << " I am not a link, nor do I have a path... preview";
    return Response{"text/html", 200, doc_->html(), to_here};
  }
  auto nxt = path.pop();
  to_here.append("/").append(nxt);
  auto it = std::find_if(links_.begin(), links_.end(), [nxt](auto&l){return l.first==nxt;});
  if (links_.end() != it) {
    auto& link = it->second;
    if (!link.node) {
      link.node = blu(link.cid);
    }
    if (link.node) {
      LOG(INFO) << to_here << " CBOR node: descending into '" << nxt
                << "' = " << link.cid;
      return link.node->resolve(path, blu, to_here);
    }
  } else if (auto child = doc_->at(nxt)) {
    auto child_node = std::make_shared<Self>(std::move(child));
    Link link2child{"",child_node};
    links_.emplace_back(nxt, link2child);
    LOG(INFO) << "Descending into newly-created child node for map entry '"
              << nxt << "', path is now '" << path.to_string() << "'.";
    return links_.back().second.node->resolve(path, blu, to_here);
  }
  return ProvenAbsent{};
}

Self::DagCborNode(std::unique_ptr<Data> p) : doc_{std::move(p)} {}
Self::~DagCborNode() {}
