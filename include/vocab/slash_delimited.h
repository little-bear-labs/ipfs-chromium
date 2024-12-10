#ifndef IPFS_SLASH_DELIMITED_H_
#define IPFS_SLASH_DELIMITED_H_

#include <iosfwd>
#include <string>
#include <string_view>

namespace google::protobuf::internal {
class LogMessage;
}

namespace ipfs {
/*! A block of ASCII text that is /-delimited, which one may want to split on /
 */
struct SlashDelimited {
  std::string_view remainder_;

 public:
  SlashDelimited() : remainder_{""} {}
  /*! @param unowned Text to parse into /-delimited components
   */
  explicit SlashDelimited(std::string_view unowned);
  explicit operator bool() const;
  std::string_view pop();
  std::string_view pop_all();
  std::string_view pop_n(std::size_t);
  std::string_view peek_back() const;
  std::string pop_back();
  std::string to_string() const { return std::string{remainder_}; }
  std::string_view to_view() const { return remainder_; }
};
}  // namespace ipfs

std::ostream& operator<<(std::ostream&, ipfs::SlashDelimited const&);
google::protobuf::internal::LogMessage& operator<<(
    google::protobuf::internal::LogMessage&,
    ipfs::SlashDelimited const&);

#endif  // IPFS_SLASH_DELIMITED_H_
