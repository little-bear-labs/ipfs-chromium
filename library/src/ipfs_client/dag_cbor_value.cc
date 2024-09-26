#include <ipfs_client/dag_cbor_value.h>

#include <vocab/html_escape.h>


#include <ostream>
#include <ios>
#include <iomanip>
#include <cstdint>
#include <sstream>
#include <string>

using Self = ipfs::DagCborValue;

void Self::html(std::ostream& str) const {
  if (auto u = as_unsigned()) {
    str << "<b class='cbor_unsigned'>" << *u << "</b>\n";
  } else if (auto si = as_signed()) {
    str << "<b class='cbor_signed'>" << *si << "</b>\n";
  } else if (auto fl = as_float()) {
    str << "<b class='cbor_float'>" << *si << "</b>\n";
  } else if (auto s = as_string()) {
    str << "<p class='cbor_string'><em>&quot;</em>";
    for (auto c : *s) {
      str << html_escape(c);
    }
    str << "<em>&quot;</em></p>\n";
  } else if (auto cid = as_link()) {
    auto cs = cid.value().to_string();
    if (static_cast<unsigned int>(!cs.empty()) != 0U) {
      str << "<a class='cbor_link' href='ipfs://" << cs << "'>" << cs
          << "</a>\n";
    } else {
      str << "<h3 class='cbor_broken_link'>Unhandled CID in Link</h3>\n";
    }
  } else if (auto bin = as_bytes()) {
    str << "<p class='cbor_byte_string'>0x";
    for (auto b : *bin) {
      str << ' ' << std::hex << std::setw(2) << std::setfill('0')
          << static_cast<std::uint16_t>(b);
    }
    str << "</p>\n";
  } else if (is_array()) {
    str << "<ol class='cbor_array'>\n";
    iterate_array([&str](auto& v) {
      str << "  <li>\n";
      v.html(str);
      str << "  </li>\n";
    });
    str << "</ol>\n";
  } else if (is_map()) {
    str << "<table class='cbor_map' border=1>\n";
    iterate_map([&str](auto k, auto& v) {
      str << "  <tr><td>" << k << "</td><td>\n";
      v.html(str);
      str << "  </td></tr>\n";
    });
    str << "</table>\n";
  } else if (auto bul = as_bool()) {
    const auto *val = (bul.value() ? "True" : "False");
    str << " <b class='cbor_bool'>" << val << "</b>\n";
  } else {
    str << "<b class='cbor_null' style='color:red;'>&empty;</b>\n";
  }
}

auto Self::html() const -> std::string {
  std::ostringstream oss;
  oss << "<html><title>DAG-CBOR Preview</title><body>\n";
  html(oss);
  oss << "</body></html>";
  return oss.str();
}
