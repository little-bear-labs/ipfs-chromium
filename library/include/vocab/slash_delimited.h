#ifndef IPFS_SLASH_DELIMITED_H_
#define IPFS_SLASH_DELIMITED_H_

#include <string>
#include <string_view>

namespace ipfs {
struct SlashDelimited {
  std::string_view remainder_;

 public:
  SlashDelimited(std::string_view unowned);
  explicit operator bool() const;
  std::string_view pop();
  std::string_view pop_all();
  std::string_view pop_n(std::size_t);
  std::string_view peek_back() const;
  std::string to_string() const { return std::string{remainder_}; }
};
}  // namespace ipfs

#endif  // IPFS_SLASH_DELIMITED_H_
