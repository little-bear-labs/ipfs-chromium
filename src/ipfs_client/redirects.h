#ifndef IPFS_REDIRECTS_H_
#define IPFS_REDIRECTS_H_

#include <cstdint>

#include <string>
#include <string_view>
#include <vector>

namespace ipfs ::redirects {
/*! A directive (a non-empty line without comments) inside a _redirects file
 */
class Directive {
  enum class ComponentType { LITERAL, PLACEHOLDER, SPLAT };
  std::vector<std::pair<ComponentType, std::string>> components_;
  std::string to_;
  int const status_;

 public:
  /*! @param from path/glob to match against
   *  @param to partial path to replace from with
   *  @param status HTTP status to use in successful response that hit this rule
   */
  Directive(std::string_view from, std::string_view to, int status);
  std::uint16_t rewrite(std::string&) const;
  std::string error() const;
  bool valid() const { return error().empty(); }
};
/*! A _redirects file
 */
class File {
  std::vector<Directive> directives_;
  std::string error_;

 public:
  explicit File(std::string_view to_parse);

  bool valid() const { return error().empty(); }
  std::string const& error() const { return error_; }
  std::uint16_t rewrite(std::string& missing_path) const;

 private:
  bool parse_line(std::string_view, int);
};
}  // namespace ipfs::redirects

#endif  // IPFS_REDIRECTS_H_
