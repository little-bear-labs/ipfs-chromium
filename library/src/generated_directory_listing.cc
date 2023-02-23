#include "generated_directory_listing.h"

ipfs::GeneratedDirectoryListing::GeneratedDirectoryListing(
    std::string_view base_path)
    : html_("<html>\n  <title>"), base_path_(base_path) {
  if (base_path.empty() || base_path[0] != '/') {
    base_path_.insert(0UL, 1UL, '/');
  }
  if (base_path_.back() != '/') {
    base_path_.push_back('/');
  }
  html_.append(base_path_)
      .append(" (directory listing)</title>\n")
      .append("  <body>\n")
      .append("    <ul>\n");
  if (base_path.find_first_not_of("/") < base_path.size()) {
    std::string_view dotdotpath{base_path_};
    dotdotpath.remove_suffix(1);  // Remove that trailing /
    auto last_slash = dotdotpath.find_last_of("/");
    dotdotpath = dotdotpath.substr(0, last_slash + 1UL);
    AddLink("..", dotdotpath);
  }
}

void ipfs::GeneratedDirectoryListing::AddEntry(std::string_view name) {
  auto path = base_path_;
  path.append(name);
  AddLink(name, path);
}
void ipfs::GeneratedDirectoryListing::AddLink(std::string_view name,
                                              std::string_view path) {
  html_.append("      <li>\n")
      .append("        <a href='")
      .append(path)
      .append("'>")
      .append(name)
      .append("</a>\n")
      .append("      </li>\n");
}

std::string const& ipfs::GeneratedDirectoryListing::Finish() {
  return html_.append("    </ul>\n").append("  </body>\n").append("</html>\n");
}