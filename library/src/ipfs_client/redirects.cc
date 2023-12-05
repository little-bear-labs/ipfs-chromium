#include "redirects.h"

#include "log_macros.h"

#include <vocab/slash_delimited.h>

#include <numeric>
#include <unordered_set>

namespace r = ipfs::redirects;
using namespace std::literals;

namespace {
// 2.4.4 Max File Size
// The file size must not exceed 64 KiB.
constexpr std::size_t MAX_SIZE = 64UL * 1024UL * 1024UL;

// Not including \n which terminates lines
constexpr std::string_view WHITESPACE = " \t\f\r\v\n";

// https://specs.ipfs.tech/http-gateways/web-redirects-file/#status
constexpr int DEFAULT_STATUS = 301;
// https://specs.ipfs.tech/http-gateways/web-redirects-file/#error-handling
constexpr int PARSE_ERROR_STATUS = 500;
}  // namespace

r::Directive::Directive(std::string_view from, std::string_view to, int status)
    : to_{to}, status_{status} {
  SlashDelimited comp_str_s{from};
  std::unordered_set<std::string_view> placeholders;
  while (comp_str_s) {
    auto comp_str = comp_str_s.pop();
    if (comp_str.empty()) {
      LOG(ERROR) << "Got empty slash-delimited component. Should not have.";
      return;
    } else if (comp_str == "*") {
      components_.emplace_back(ComponentType::SPLAT, comp_str);
    } else if (comp_str[0] == ':') {
      if (placeholders.insert(comp_str).second) {
        components_.emplace_back(ComponentType::PLACEHOLDER, comp_str);
      } else {
        to_.assign("ERROR: Duplicate placeholder ").append(comp_str);
        return;
      }
    } else {
      components_.emplace_back(ComponentType::LITERAL, comp_str);
    }
  }
}
std::uint16_t r::Directive::rewrite(std::string& path) const {
  auto input = SlashDelimited{path};
  auto result = to_;
  auto replace = [&result](std::string_view ph, std::string_view val) {
    std::size_t pos;
    while ((pos = result.find(ph)) < result.size()) {
      result.replace(pos, ph.size(), val);
    }
  };
  for (auto [type, comp_str] : components_) {
    if (!input) {
      VLOG(2) << "Ran out of input in [" << path
              << "] before running out of pattern components to match against "
                 "(was looking for ["
              << comp_str << "]. Not a match.";
      return 0;
    }
    if (type == ComponentType::LITERAL) {
      if (comp_str != input.pop()) {
        return 0;
      }
    } else if (type == ComponentType::PLACEHOLDER) {
      replace(comp_str, input.pop());
    } else {
      replace(":splat"sv, input.pop_all());
    }
  }
  if (input) {
    return 0;
  } else {
    path = result;
    return status_;
  }
}
std::string r::Directive::error() const {
  if (starts_with(to_, "ERROR: ")) {
    return to_;
  }
  if (status_ < 200 || status_ > 451) {
    return "UNSUPPORTED STATUS " + std::to_string(status_);
  }
  if (components_.empty()) {
    return "Empty directive pattern";
  }
  if (to_.empty()) {
    return "Empty redirect target location";
  }
  if (to_.at(0) != '/' && to_.find("://") == std::string::npos) {
    return "Location must begin with / or be a URL";
  }
  return {};
}

std::uint16_t r::File::rewrite(std::string& missing_path) const {
  for (auto& directive : directives_) {
    auto status = directive.rewrite(missing_path);
    if (status) {
      return status;
    }
  }
  return 0;
}
r::File::File(std::string_view to_parse) {
  if (to_parse.size() > MAX_SIZE) {
    error_ = "INPUT FILE TOO LARGE " + std::to_string(to_parse.size());
    return;
  }
  for (auto line_number = 1; valid() && to_parse.size(); ++line_number) {
    auto line_end = to_parse.find('\n');
    auto line = to_parse.substr(0UL, line_end);
    if (!parse_line(line, line_number)) {
      LOG(INFO) << "Line #" << line_number << " ignored: [" << line << ']';
    } else if (directives_.empty()) {
      LOG(ERROR) << "Expected to have a directive after parsing line #"
                 << line_number << ": " << line;
    } else if (directives_.back().valid()) {
      VLOG(1) << "Line #" << line_number << " parsed. " << line;
    } else {
      error_ = "FAILURE PARSING LINE # " + std::to_string(line_number);
      error_.append(": ")
          .append(directives_.back().error())
          .append(" [")
          .append(line)
          .push_back(']');
      LOG(ERROR) << error_;
      return;
    }
    if (line_end < to_parse.size()) {
      to_parse.remove_prefix(line_end + 1);
    } else {
      break;
    }
  }
  if (directives_.empty()) {
    error_ = "No redirection directives in _redirects";
    LOG(ERROR) << error_;
  }
}

namespace {
std::pair<int, std::string> parse_status(std::string_view line,
                                         std::size_t col);
}
bool r::File::parse_line(std::string_view line, int line_number) {
  if (line.empty()) {
    // empty line is not a directive
    return false;
  }
  auto bpos = line.find_first_not_of(WHITESPACE);
  if (bpos == std::string_view::npos) {
    // effectively empty line
    return false;
  } else if (line[bpos] == '#') {
    // https://specs.ipfs.tech/http-gateways/web-redirects-file/#comments
    return false;
  }
  auto epos = line.find_first_of(WHITESPACE, bpos);
  if (epos == std::string_view::npos) {
    error_ = "Parsing _redirects file: line # " + std::to_string(line_number);
    error_
        .append(" , expected at least 2 tokens (from and to) for directive: [")
        .append(line)
        .append("], but didn't even get whitespace to end from");
    return false;
  }
  auto from = line.substr(bpos, epos - bpos);
  bpos = line.find_first_not_of(WHITESPACE, epos);
  if (bpos == std::string_view::npos) {
    error_ = "Parsing _redirects file: line # " + std::to_string(line_number);
    error_
        .append(" , expected at least 2 tokens (from and to) for directive: [")
        .append(line)
        .append("], but didn't get a to");
    return false;
  }
  epos = line.find_first_of(WHITESPACE, bpos);
  auto to = line.substr(bpos, epos - bpos);
  auto [status, err] = parse_status(line, epos);
  if (err.empty()) {
    directives_.emplace_back(from, to, status);
    return true;
  } else {
    error_ = err;
    LOG(ERROR) << "Error parsing status on line #" << line_number << " ["
               << line << "].";
    return false;
  }
}

namespace {

std::pair<int, std::string> parse_status(std::string_view line,
                                         std::size_t col) {
  if (col >= line.size()) {
    VLOG(2) << " No status specified, using default.";
    return {DEFAULT_STATUS, ""};
  }
  auto b = line.find_first_not_of(WHITESPACE, col);
  if (b >= line.size()) {
    VLOG(2)
        << " No status specified (line ended in whitespace), using default.";
    return {DEFAULT_STATUS, ""};
  }
  auto status_str = line.substr(b);
  if (status_str.size() < 3) {
    return {PARSE_ERROR_STATUS,
            " Not enough characters for a valid status string: [" +
                std::string{status_str} + "]."};
  }
  auto good = [](int i) { return std::make_pair(i, ""s); };
  auto unsupported = [status_str]() {
    return std::make_pair(
        PARSE_ERROR_STATUS,
        "Unsupported status specified in directive:" + std::string{status_str});
  };
  /*
   * 200 - OK treated as a rewrite, without changing the URL in the browser.
   * 301 - Permanent Redirect (default)
   * 302 - Found (commonly used for Temporary Redirect)
   * 303 - See Other (replacing PUT and POST with GET)
   * 307 - Temporary Redirect (explicitly preserving body and HTTP method)
   * 308 - Permanent Redirect (preserving body & method of original request)
   * 404 - Not Found (Useful for a pretty 404 page)
   * 410 - Gone
   * 451 - Unavailable For Legal Reasons
   */
  switch (status_str[0]) {
    case '2':
      return status_str == "200" ? good(200) : unsupported();
    case '3':
      if (status_str[1] != '0') {
        return unsupported();
      }
      return good(300 + status_str[2] - '0');
    case '4':
      switch (status_str[1]) {
        case '0':
          return status_str[2] == '4' ? good(404) : unsupported();
        case '1':
          return status_str[2] == '0' ? good(410) : unsupported();
        case '5':
          return status_str[2] == '1' ? good(451) : unsupported();
        default:
          return unsupported();
      }
    default:
      return unsupported();
  }
}
}  // namespace
