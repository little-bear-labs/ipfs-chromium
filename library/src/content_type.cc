#include <ipfs_client/content_type.h>

#include <iostream>

std::string ipfs::GuessContentType(
    std::string_view filename,
    std::string_view /*todo libmagic type stuff*/) {
  std::clog << "GuessContentType(" << filename << ")\n";
  if (filename.empty() || filename.back() == '/') {
    return "text/directory";
  }
  auto dot = filename.find_last_of(".");
  if (dot == std::string_view::npos || dot == filename.size() - 1) {
    return "text/plain";  // TODO
  }
  auto ext = filename.substr(dot + 1);
  for (auto [alias, normal] :
       {std::pair{"htm", "html"}, {"faq", "txt"}, {"jpg", "jpeg"}}) {
    if (ext == alias) {
      ext = normal;
    }
  }
  if (ext == "htm") {
    return "text/html";
  }
  if (ext == "js") {
    return "text/javascript";
  }
  if (ext == "txt") {
    return "text/plain; charset=utf-8";
  }
  using namespace std::literals;
  for (auto txt : {"html"sv, "css"sv, "csv"sv}) {
    if (txt == ext) {
      std::string result{"text/"};
      return result.append(txt);
    }
  }
  for (auto img : {"png"sv, "jpeg"sv, "gif"sv}) {
    if (img == ext) {
      std::string result{"image/"};
      return result.append(img);
    }
  }
  std::clog << "TODO: handle mime type for " << filename << std::endl;
  return "text/html";
}
