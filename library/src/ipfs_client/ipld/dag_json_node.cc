#include "dag_json_node.h"

#include <vocab/html_escape.h>
#include <libp2p/multi/content_identifier_codec.hpp>

using Self = ipfs::ipld::DagJsonNode;
using CidCodec = libp2p::multi::ContentIdentifierCodec;

Self::DagJsonNode(std::unique_ptr<DagJsonValue> j) : data_(std::move(j)) {
  auto cid = data_->get_if_link();
  if (!cid) {
    return;
  }
  auto cid_str = CidCodec::toString(cid.value());
  if (cid_str.has_value()) {
    links_.emplace_back("", Link(cid_str.value()));
  }
}
Self::~DagJsonNode() noexcept {}

auto Self::resolve(SlashDelimited path,
                   BlockLookup blu,
                   std::string& up_to_here) -> ResolveResult {
  if (auto link = is_link()) {
    return child_resolve(path, blu, up_to_here, *link);
  }
  if (!path) {
    return Response{"text/html", 200, html(), up_to_here};
  }
  auto nxt = path.pop();
  for (auto& [k, l] : links_) {
    if (k == nxt) {
      return child_resolve(path, blu, up_to_here, l);
    }
  }
  auto child_data = (*data_)[nxt];
  if (child_data) {
    links_.emplace_back(
        nxt, Link("", std::make_shared<DagJsonNode>(std::move(child_data))));
    return child_resolve(path, blu, up_to_here, links_.back().second);
  }
  return ProvenAbsent{};
}
auto Self::child_resolve(SlashDelimited path,
                         BlockLookup blu,
                         std::string& up_to_here,
                         Link& l) -> ResolveResult {
  if (!l.node) {
    l.node = blu(l.cid);
  }
  if (!l.node) {
    return MoreDataNeeded{{"/ipfs/" + l.cid}};
  }
  return l.node->resolve(path, blu, up_to_here);
}
auto Self::is_link() -> Link* {
  if (links_.size() == 1UL && links_.front().first.empty()) {
    return &links_.front().second;
  } else {
    return nullptr;
  }
}
namespace {
void write_body(std::ostream& str, ipfs::DagJsonValue const& val) {
  if (auto link = val.get_if_link()) {
    auto cid_str = CidCodec::toString(link.value());
    if (cid_str.has_value()) {
      str << "<a href='ipfs://" << cid_str.value() << "'>" << cid_str.value()
          << "</a>\n";
    } else {
      str << "<strong><em>Broken link</em></strong>\n";
    }
  } else if (auto keys = val.object_keys()) {
    str << "{<table>\n";
    for (auto& key : keys.value()) {
      str << "  <tr>\n    <td>&nbsp;&nbsp;</td>\n"
          << "    <td style='vertical-align:top'>&quot;";
      for (auto c : key) {
        str << html_escape(c);
      }
      str << "&quot;:</td>\n    <td>\n";
      auto child = val[key];
      write_body(str, *child);
      str << ",    </td>\n  </tr>\n";
    }
    str << "</table>}\n";
  } else if (val.iterate_list([](auto&) {})) {
    str << "[<table>\n";
    val.iterate_list([&str](auto& child) {
      str << "  <tr>\n    <td>&nbsp;&nbsp;</td>\n    <td>\n";
      write_body(str, child);
      str << "  </td>\n  </tr>\n";
    });
    str << "</table>]\n";
  } else {
    auto plain = val.pretty_print();
    //    str << "<p>";
    for (auto c : plain) {
      if (c == '\n') {
        str << "<br />\n";
      } else {
        str << html_escape(c);
      }
    }
    //    str << "</p>\n";
  }
}
}  // namespace
std::string const& Self::html() {
  if (html_.empty()) {
    std::ostringstream html;
    html << "<html><title>Preview of DAG-JSON</title><body>\n";
    write_body(html, *data_);
    html << "\n</body></html>";
    html_ = html.str();
  }
  return html_;
}