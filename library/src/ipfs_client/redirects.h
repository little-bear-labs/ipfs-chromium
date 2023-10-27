#ifndef IPFS_REDIRECTS_H_
#define IPFS_REDIRECTS_H_

#include <cstdint>

#include <string>
#include <string_view>
#include <vector>

namespace ipfs {
namespace redirects {
class Directive {
  enum class ComponentType { LITERAL, PLACEHOLDER, SPLAT };
  std::vector<std::pair<ComponentType, std::string>> components_;
  std::string const to_;
  int const status_;

 public:
  Directive(std::string_view, std::string_view, int);
  std::uint16_t rewrite(std::string&) const;
  std::string error() const;
  bool valid() const { return error().empty(); }
};
class File {
  std::vector<Directive> directives_;
  std::string error_;

 public:
  File(std::string_view to_parse);

  bool valid() const { return error().empty(); }
  std::string const& error() const { return error_; }
  std::uint16_t rewrite(std::string& missing_path) const;

 private:
  bool parse_line(std::string_view, int);
};
}  // namespace redirects
}  // namespace ipfs

#endif  // IPFS_REDIRECTS_H_
