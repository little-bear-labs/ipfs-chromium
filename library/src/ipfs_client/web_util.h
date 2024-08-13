#ifndef IPFS_CHROMIUM_WEB_UTIL_H
#define IPFS_CHROMIUM_WEB_UTIL_H

#include <string>
#include <string_view>

namespace ipfs::util {
std::string TrivialMimeGuess(std::string extension,
                             std::string_view content,
                             std::string const& url);
std::string RoughlyUnescapeUrlComponent(std::string_view url_comp);
}  // namespace ipfs::util

#endif  // IPFS_CHROMIUM_WEB_UTIL_H
