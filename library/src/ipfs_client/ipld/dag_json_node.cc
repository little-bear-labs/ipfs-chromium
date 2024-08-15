#include "dag_json_node.h"

#include <vocab/html_escape.h>

#include <sstream>

using Self = ipfs::ipld::DagJsonNode;

Self::DagJsonNode(std::unique_ptr<DagJsonValue> j) : data_(std::move(j)) {
  auto cid = data_->get_if_link();
  if (!cid) {
    return;
  }
  auto cid_str = cid->to_string();
  if (cid_str.size()) {
    links_.emplace_back("", Link(cid_str));
  }
}
Self::~DagJsonNode() noexcept = default;

auto Self::resolve(ResolutionState& params) -> ResolveResult {
  auto respond_as_link = CallChild(params, "");
  if (!std::get_if<ProvenAbsent>(&respond_as_link)) {
    return respond_as_link;
  }
  if (params.IsFinalComponent()) {
    return Response::html(html(), params.MyPath().to_string());
  }
  return CallChild(params, [this](std::string_view name) -> NodePtr {
    auto child_data = (*data_)[name];
    if (child_data) {
      return std::make_shared<DagJsonNode>(std::move(child_data));
    }
    return {};
  });
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
    auto cid_str = link.value().to_string();
    str << "<a href='ipfs://" << cid_str << "'>" << cid_str << "</a>\n";
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