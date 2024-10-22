#include "web_util.h"
#include <string>
#include <string_view>
#include <cctype>

namespace u = ipfs::util;

auto u::TrivialMimeGuess(std::string ext,
                                std::string_view,
                                std::string const&) -> std::string {
  if (ext.starts_with("htm")) {
    return "text/html";
  }
  if (ext == "json" || ext == "txt" || ext == "css") {
    return "text/" + ext;
  }
  if (ext.starts_with("jp")) {
    return "image/jpeg";
  }
  if (ext.size() == 3 && ext[2] == 'g') {
    return "image/" + ext;
  }
  return "application/octet-stream";
}
auto u::RoughlyUnescapeUrlComponent(std::string_view url_comp) -> std::string {
  std::string rv{url_comp};
  auto xval = [](char c) {
    if (c <= '9') {
      return c - '0';
    }
    if (c <= 'F') {
      return c - 'A' + 10;
    }
    return c - 'a' + 10;
  };
  for (auto i = 0UL; i + 1UL < rv.size(); ++i) {
    if (rv[i] != '%') {
      continue;
    }
    auto a = rv[i + 1UL];
    if (rv[i + 1UL] == '%') {
      rv.erase(i, 1UL);
      continue;
    }
    if (i + 2UL >= rv.size()) {
      break;
    }
    if (std::isxdigit(a) == 0) {
      continue;
    }
    auto b = rv[i + 2UL];
    if (std::isxdigit(b) != 0) {
      rv[i] = xval(a) * 16 + xval(b);
      rv.erase(i + 1UL, 2);
    }
  }
  return rv;
}
