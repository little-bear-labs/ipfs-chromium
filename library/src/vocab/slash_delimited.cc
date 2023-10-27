#include <vocab/slash_delimited.h>

using Self = ipfs::SlashDelimited;

Self::SlashDelimited(std::string_view unowned) : remainder_{unowned} {}

Self::operator bool() const {
  return remainder_.find_first_not_of("/") < remainder_.size();
}
std::string_view Self::pop() {
  if (remainder_.empty()) {
    return remainder_;
  }
  auto slash = remainder_.find('/');
  if (slash == std::string_view::npos) {
    return pop_all();
  }
  auto result = remainder_.substr(0UL, slash);
  remainder_.remove_prefix(slash + 1);
  if (result.empty()) {
    return pop();
  } else {
    return result;
  }
}
std::string_view Self::pop_all() {
  auto result = remainder_;
  remainder_ = "";
  return result;
}
std::string_view Self::pop_n(std::size_t n) {
  std::size_t a = 0UL;
  while (n) {
    auto slash = remainder_.find('/', a);
    auto non_slash = remainder_.find_first_not_of("/", a);
    if (slash == std::string_view::npos) {
      auto result = remainder_;
      remainder_ = "";
      return result;
    }
    if (non_slash < slash) {
      --n;
    }
    a = slash + 1UL;
  }
  auto result = remainder_.substr(0UL, a - 1);
  remainder_.remove_prefix(a);
  return result;
}
std::string_view Self::peek_back() const {
  auto s = remainder_;
  while (!s.empty() && s.back() == '/') {
    s.remove_suffix(1);
  }
  if (s.empty()) {
    return s;
  }
  auto last_slash = s.find_last_of('/');
  if (last_slash < remainder_.size()) {
    return remainder_.substr(last_slash + 1);
  } else {
    return s;
  }
}
