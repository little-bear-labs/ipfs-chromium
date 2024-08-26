#include "web_util.h"

namespace u = ipfs::util;

std::string u::TrivialMimeGuess(std::string ext,
                                std::string_view,
                                std::string const&) {
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
std::string u::RoughlyUnescapeUrlComponent(std::string_view url_comp) {
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
    if (!std::isxdigit(a)) {
      continue;
    }
    auto b = rv[i + 2UL];
    if (std::isxdigit(b)) {
      rv[i] = xval(a) * 16 + xval(b);
      rv.erase(i + 1UL, 2);
    }
  }
  return rv;
}
