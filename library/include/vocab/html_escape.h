#ifndef IPFS_HTML_ESCAPE_H_
#define IPFS_HTML_ESCAPE_H_

#include <string_view>

constexpr std::string_view html_escape(char& c) {
  switch (c) {
    case '"':
      return "&quot;";
    case '\'':
      return "&apos;";
    case '<':
      return "&lt;";
    case '>':
      return "&gt;";
    case '&':
      return "&amp;";
    default:
      return {&c, 1UL};
  }
}

#endif  // IPFS_HTML_ESCAPE_H_
