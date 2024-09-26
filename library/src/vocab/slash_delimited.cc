#include <vocab/slash_delimited.h>

#include <cstddef>

#include <iostream>
#include <string>
#include <string_view>

using Self = ipfs::SlashDelimited;

Self::SlashDelimited(std::string_view unowned) : remainder_{unowned} {}

Self::operator bool() const {
  return remainder_.find_first_not_of('/') < remainder_.size();
}
auto Self::pop() -> std::string_view {
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
auto Self::pop_all() -> std::string_view {
  auto result = remainder_;
  remainder_ = "";
  return result;
}
auto Self::pop_n(std::size_t n) -> std::string_view {
  std::size_t idx = 0UL;
  while (n > 0UL) {
    auto slash = remainder_.find('/', idx);
    auto non_slash = remainder_.find_first_not_of('/', idx);
    if (slash == std::string_view::npos) {
      auto result = remainder_;
      remainder_ = "";
      return result;
    }
    if (non_slash < slash) {
      --n;
    }
    idx = slash + 1UL;
  }
  auto result = remainder_.substr(0UL, idx - 1);
  remainder_.remove_prefix(idx);
  return result;
}
auto Self::peek_back() const -> std::string_view {
  auto rem = remainder_;
  while (!rem.empty() && rem.back() == '/') {
    rem.remove_suffix(1);
  }
  if (rem.empty()) {
    return rem;
  }
  auto last_slash = rem.find_last_of('/');
  if (last_slash < rem.size()) {
    rem.remove_prefix(last_slash + 1);
  }
  return rem;
}
auto Self::pop_back() -> std::string {
  auto non_slash = remainder_.find_last_not_of('/');
  if (non_slash == std::string_view::npos) {
    return "";
  }
  auto slash = remainder_.find_last_of('/', non_slash);
  std::string result;
  if (slash == std::string_view::npos) {
    result = remainder_;
    remainder_ = "";
  } else {
    result = remainder_.substr(slash + 1, non_slash - slash);
    remainder_ = remainder_.substr(0UL, slash);
  }
  return result;
}

auto operator<<(std::ostream& stream, ipfs::SlashDelimited const& sdp) -> std::ostream& {
  return stream << sdp.to_view();
}

#ifndef NACL_TC_REV
#if __has_include(<google/protobuf/stubs/logging.h>)
#include <google/protobuf/stubs/logging.h>

using namespace google::protobuf::internal;
using namespace google::protobuf;

#if PROTOBUF_VERSION >= 3020000
#include <google/protobuf/stubs/stringpiece.h>
LogMessage& operator<<(LogMessage& str, ipfs::SlashDelimited const& sdp) {
  return str << sdp.to_view();
}
#elif __has_include(<google/protobuf/stubs/stringpiece.h>)
#include <google/protobuf/stubs/stringpiece.h>
auto operator<<(LogMessage& str, ipfs::SlashDelimited const& sdp) -> LogMessage& {
  return str << StringPiece{sdp.to_view()};
}
#else
LogMessage& operator<<(LogMessage& str, ipfs::SlashDelimited const& sdp) {
  return str << std::string{sdp.to_view()};
}
#endif

#endif
#endif
